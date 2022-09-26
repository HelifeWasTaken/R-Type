#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <boost/container/vector.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/variant.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/move/unique_ptr.hpp>
#include <boost/atomic.hpp>
#include <hl/Silva/sparse_array>

class UDPClient {
private:
	boost::asio::ip::udp::resolver _resolver;
	boost::asio::ip::udp::resolver::query _query;
	boost::asio::ip::udp::endpoint _receiver_endpoint;
	boost::asio::ip::udp::socket _socket;
	boost::asio::ip::udp::endpoint _sender_endpoint;

public:
	UDPClient(boost::asio::io_service& io_service, const char *host)
		: _resolver(io_service)
		, _query(boost::asio::ip::udp::v4(), host, "daytime")
		, _receiver_endpoint(*_resolver.resolve(_query))
		, _socket(io_service)
	{
		_socket.open(boost::asio::ip::udp::v4());
	}

	size_t receive_from(void *data, const size_t size)
	{
		return _socket.receive_from(boost::asio::buffer(data, size), _sender_endpoint);
	}

	size_t send_to(const void* data, const size_t size)
	{
		return _socket.send_to(boost::asio::buffer(data, size), _receiver_endpoint);
	}
};

class TCPClient {
private:
	boost::asio::ip::tcp::resolver _resolver;
	boost::asio::ip::tcp::resolver::query _query;
	boost::asio::ip::tcp::resolver::iterator _endpoint_iterator;
	boost::asio::ip::tcp::socket _socket;

public:
	TCPClient(boost::asio::io_service& io_service, const char* host)
		: _resolver(io_service)
		, _query(host, "daytime")
		, _endpoint_iterator(_resolver.resolve(_query))
		, _socket(io_service)
	{
		boost::system::error_code error = boost::asio::error::host_not_found;
		boost::asio::ip::tcp::resolver::iterator end;

		while (error && _endpoint_iterator != end) {
			_socket.close();
			_socket.connect(*_endpoint_iterator++, error);
		}
		if (error)
			throw boost::system::system_error(error);
	}

	size_t receive(void* data, const size_t size)
	{
		return _socket.receive(boost::asio::buffer(data, size));
	}

	size_t send(const void* data, const size_t size)
	{
		return _socket.send(boost::asio::buffer(data, size));
	}
};

class UDP_TCP_Client {
private:
	boost::asio::io_service _tcp_io_service;
	boost::asio::io_service _udp_io_service;

	UDPClient _udp_client;
	TCPClient _tcp_client;

public:
	UDP_TCP_Client(const char* host)
		: _tcp_io_service(boost::asio::io_service())
		, _udp_io_service(boost::asio::io_service())
		, _udp_client(_udp_io_service, host)
		, _tcp_client(_tcp_io_service, host)
	{}

	UDPClient& udp() { return _udp_client; }
	TCPClient& tcp() { return _tcp_client; }
};

///////////////////////////////////////////////////////////////////
////////////////////////// Server Side ////////////////////////////
///////////////////////////////////////////////////////////////////

class ServerEvent {
public:
	enum ServerEventType {
		// UDP_DISCONNECTION,
		// UDP_CONNECTION,
		TCP_DISCONNECTION,
		TCP_CONNECTION,
		UDP_MESSAGE,
		TCP_MESSAGE,
		INVALID
	};
	
	using ServerEventContainer = boost::variant<
		size_t,
		boost::container::vector<char>,
		void *
	>;

private:
	const ServerEventType _type;
	ServerEventContainer _event;

public:
	ServerEvent(ServerEventType type, ServerEventContainer&& event)
		: _type(type), _event(boost::move(event))
	{}

	ServerEvent()
		: _type(ServerEventType::INVALID)
		, _event(ServerEventContainer(nullptr))
	{}

	ServerEventType get_type() const { return _type; }
	ServerEventContainer &get_raw_event() { return _event;  }
	template<typename T>
	T &get_event() { return boost::get<T&>(_event); }
};

class Server {
private:
	template<typename Socket>
	using socket_sparse_array_t = hl::silva::sparse_array<boost::movelib::unique_ptr<Socket>>;

	// IO services
	boost::asio::io_service _tcp_io_service;
	boost::asio::ip::tcp::acceptor _tcp_acceptor;
	boost::asio::io_service _udp_io_service;

	// Sockets
	socket_sparse_array_t<boost::asio::ip::tcp::socket> _tcp_sockets;
	boost::lockfree::stack<size_t> _unused_tcp_sockets_indexes;
	boost::atomic_size_t _last_tcp_index;
	boost::mutex _tcp_sockets_mut;

	// Events
	boost::lockfree::queue<ServerEvent> _event;
	boost::atomic_bool _server_running;

	// Threads
	boost::movelib::unique_ptr<boost::thread> _thread_tcp_acceptor;
	boost::movelib::unique_ptr<boost::thread> _thread_udp_reader;
	boost::movelib::unique_ptr<boost::thread> _thread_tcp_reader;

	//
	// Utitlity verbose function for sync_disconnection
	// Is thread safe as long as the good mutex and socket_sparse_array is passed
	//
	template<typename SocketSparseArray>
	void disconnect_any_socket_sync(const size_t index, SocketSparseArray& s, boost::mutex &mut, ServerEvent::ServerEventType event_type)
	{
		boost::lock_guard<boost::mutex> lock(mut);

		// Maybe handle disconnection failure send event
		if (s.non_null(index)) {
			s.erase(index);
			_event.push(ServerEvent(event_type boost::move(ServerEvent::ServerEventContainer(index))));
                        _unused_tcp_sockets_indexes.push(index);
		}
	}

	//
	// Utitlity verbose function for sync_connection
	// Is thread safe as long as the good mutex and socket_sparse_array is passed
	//
	template<typename SocketSparseArray>
	void connect_any_socket_sync(SocketSparseArray& s, boost::mutex& mut, ServerEvent::ServerEventType event_type,
		boost::movelib::unique_ptr<boost::asio::ip::tcp::socket>&& socket)
	{
		boost::lock_guard<boost::mutex> lock(mut);
		size_t optional_index;

		if (_unused_tcp_sockets.pop(optional_index)) {
			s.insert(optional_index, boost::move(socket));
			_event.push(ServerEvent(event_type boost::move(ServerEvent::ServerEventContainer(optional_index))));
		} else {
			s.insert(_last_tcp_index, boost::move(socket));
			_event.push(ServerEvent(event_type boost::move(ServerEvent::ServerEventContainer(_last_tcp_index)))));
			++_last_tcp_index;
		}
	}

	//
	// This function should never return
	//	- is thread safe
	//  - must be called in a separate thread (never returns)
	//  - Will be used to accept indefinitely tcp_clients
	//  - Is automatically called on start() call
	//
	void accept_tcp_clients_sync()
	{
		do {
			try {
				boost::movelib::unique_ptr<boost::asio::ip::tcp::socket> socket(new boost::asio::ip::tcp::socket(_tcp_io_service));
				_tcp_acceptor.accept(*socket);
				connect_any_socket_sync(_tcp_sockets, _tcp_sockets_mut, ServerEvent::ServerEventType::TCP_CONNECTION, std::move(socket));
			} catch (...) {}
		} while (_server_running);
	}

	//
	// This function should never return
	// - is thread safe
	// - must be called in a separate thread (never returns)
	// - Will be used to read indefinitely on tcp sockets
	// - Is automatically called on start() call
	//
	void read_tcp_sockets()
	{
		// TODO: Unimplemented
		// Determine which socket to read on
		// Then add the event in the queue
		do {

		} while (_server_running);
	}

	//
	// This function should never return
	// - is thread safe
	// - must be called in a separate thread (never returns)
	// - Will be used to read indefinitely on udp sockets
	// - Is automatically called on start() call
	//
	void read_udp_sockets()
	{
		// TODO: Unimplemented
		// Read the trame of the message if any
		// Determine if it belongs to any of the tcp socket
		// If not ignore else add the message to the event queue
		do {
			
		} while (_server_running);
	}
public:
	//
	// Can be runnned in a separate thread
	// Starts the server if not already running
	// If any problem occured (notably thread problems) returns false
        // If the server is_running() member returns true this function has already been finished (or was already called)
	//
	bool start()
	{
		if (_server_running)
			return true;
		try {
			_thread_tcp_acceptor = boost::movelib::unique_ptr<boost::thread>(
				new boost::thread(std::bind(&Server::accept_tcp_clients_sync, *this))
				);

			_thread_tcp_reader = boost::movelib::unique_ptr<boost::thread>(
				new boost::thread(std::bind(&Server::read_tcp_sockets, *this))
				);

			_thread_udp_reader = boost::movelib::unique_ptr<boost::thread>(
				new boost::thread(std::bind(&Server::read_udp_sockets, *this))
				);
		} catch (...) {
			_server_running = false;
			return false;
		}
		_server_running = true;
		return true;
	}

	//
	// Stops the server if running
	// Should always return true
	// Might throw if failing to join the threads
	// Is automatically called when the Server is destroyed
	//
	bool stop()
	{
		if (!_server_running)
			return true;
		_server_running = false;
		_thread_tcp_acceptor->join();
		_thread_tcp_reader->join();
		_thread_udp_reader->join();
		return true;
	}

	Server(const int tcp_port, const int udp_port)
		: _tcp_acceptor(_tcp_io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), tcp_port))
	{
		_last_tcp_index = 0;
	}

	~Server() { stop(); }

	//
	// can be runned in a separate thread
	// Will add in the event queue the disconnection if the tcp_socket
	// was currently used and will destroy the given socket
	// Only used to disconnect manually a tcp socket for whatever reason
	//
	void disconnect_tcp_socket_sync(const size_t index)
	{
		disconnect_any_socket_sync(index, _tcp_sockets, _tcp_sockets_mut, ServerEvent::TCP_DISCONNECTION);
	}

	//
	// Polls the server for any event that might have occured
	// If no event occured returns false
	// Otherwise it copies the event in the parameter and returns true
	//
	bool poll(ServerEvent& event)
	{
		return _event.pop(event);
	}

	//
	// Tells whether the server is running
	//
	bool is_running() const
	{
		return _server_running;
	}
};