#pragma once

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/atomic.hpp>
#include <boost/container/vector.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/move/unique_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/thread.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/variant.hpp>
#include <hl/Silva/sparse_array>
#include <queue>
#include <stack>

#ifndef RTYPE_SERVER_MAX_TCP_PACKET_SIZE
#define RTYPE_SERVER_MAX_TCP_PACKET_SIZE 4096
#endif

#ifndef RTYPE_SERVER_MAX_UDP_PACKET_SIZE
#define RTYPE_SERVER_MAX_UDP_PACKET_SIZE 500
#endif

namespace rtype {
namespace net {

    using tcp_buffer_t
        = boost::array<std::uint8_t, RTYPE_SERVER_MAX_TCP_PACKET_SIZE>;
    using udp_buffer_t
        = boost::array<std::uint8_t, RTYPE_SERVER_MAX_UDP_PACKET_SIZE>;

    class IClient {
    public:
        IClient() = default;
        virtual ~IClient() = default;

        virtual size_t receive(void*, const size_t) = 0;
        virtual size_t send(const void*, const size_t) = 0;
    };

    class UDPClient : public IClient {
    private:
        boost::asio::ip::udp::resolver _resolver;
        boost::asio::ip::udp::resolver::query _query;
        boost::asio::ip::udp::endpoint _receiver_endpoint;
        boost::asio::ip::udp::socket _socket;
        boost::asio::ip::udp::endpoint _sender_endpoint;

    public:
        UDPClient(boost::asio::io_context& io_context, const char* host)
            : _resolver(io_context)
            , _query(boost::asio::ip::udp::v4(), host, "daytime")
            , _receiver_endpoint(*_resolver.resolve(_query))
            , _socket(io_context)
        {
            _socket.open(boost::asio::ip::udp::v4());
        }

        size_t receive(void* data, const size_t size) override
        {
            return _socket.receive_from(
                boost::asio::buffer(data, size), _sender_endpoint);
        }

        size_t send(const void* data, const size_t size) override
        {
            return _socket.send_to(
                boost::asio::buffer(data, size), _receiver_endpoint);
        }
    };

    class TCPClient : public IClient {
    private:
        boost::asio::ip::tcp::resolver _resolver;
        boost::asio::ip::tcp::resolver::query _query;
        boost::asio::ip::tcp::resolver::iterator _endpoint_iterator;
        boost::asio::ip::tcp::socket _socket;

    public:
        TCPClient(boost::asio::io_context& io_context, const char* host)
            : _resolver(io_context)
            , _query(host, "daytime")
            , _endpoint_iterator(_resolver.resolve(_query))
            , _socket(io_context)
        {
            boost::system::error_code error
                = boost::asio::error::host_not_found;
            boost::asio::ip::tcp::resolver::iterator end;

            while (error && _endpoint_iterator != end) {
                _socket.close();
                _socket.connect(*_endpoint_iterator++, error);
            }
            if (error)
                throw boost::system::system_error(error);
        }

        size_t receive(void* data, const size_t size) override
        {
            return _socket.receive(boost::asio::buffer(data, size));
        }

        size_t send(const void* data, const size_t size) override
        {
            return _socket.send(boost::asio::buffer(data, size));
        }
    };

    class UDP_TCP_Client {
    private:
        boost::asio::io_context _tcp_io_context;
        boost::asio::io_context _udp_io_context;

        UDPClient _udp_client;
        TCPClient _tcp_client;

    public:
        UDP_TCP_Client(const char* host)
            : _tcp_io_context(boost::asio::io_context())
            , _udp_io_context(boost::asio::io_context())
            , _udp_client(_udp_io_context, host)
            , _tcp_client(_tcp_io_context, host)
        {
        }

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

        struct ServerMessageTCP {
            size_t id;
            size_t used;
            tcp_buffer_t buffer;
        };

        struct ServerMessageUDP {
            size_t id;
            size_t used;
            udp_buffer_t buffer;
        };

        using ServerEventContainer
            = boost::variant<size_t, ServerMessageTCP, ServerMessageUDP, void*>;

    private:
        ServerEventType _type;
        ServerEventContainer _event;

    public:
        ServerEvent(ServerEventType type, ServerEventContainer&& event)
            : _type(type)
            , _event(std::move(event))
        {
        }

        ServerEvent()
            : _type(ServerEventType::INVALID)
            , _event(ServerEventContainer(nullptr))
        {}

        ~ServerEvent() = default;

        ServerEvent(const ServerEvent& e) = default;

        ServerEvent& operator=(const ServerEvent& e) = default;

        ServerEvent(ServerEvent&&) = default;
        ServerEvent& operator=(ServerEvent&&) = default;

        ServerEventType get_type() const { return _type; }
        ServerEventContainer& get_raw_event() { return _event; }
        template <typename T> T& get_event() { return boost::get<T&>(_event); }
    };

    class Server {
    public:
        template <typename Socket>
        using socket_sparse_array_t = hl::silva::sparse_array<Socket>;

        struct ClientContext {
            boost::asio::ip::tcp::socket socket;
            tcp_buffer_t buffer;
            std::unique_ptr<std::thread> reader_thread;
            std::unique_ptr<std::thread> writer_thread;
            std::atomic_bool is_reading;
            std::atomic_bool is_writing;
            std::atomic_bool should_close;

            ClientContext(boost::asio::io_context& service)
                : socket(service)
            {
                is_reading = false;
                should_close = false;
            }

            ~ClientContext() = default;
            ClientContext(const ClientContext&) = default;
            ClientContext(ClientContext&&) = default;
        };

        using shared_client_context_t = std::shared_ptr<ClientContext>;

    private:
        // IO services
        boost::asio::io_context _tcp_io_context;
        boost::asio::ip::tcp::acceptor _tcp_acceptor;

        boost::asio::io_context _udp_io_context;
        boost::asio::ip::udp::socket _server_udp_socket;

        // Sockets
        socket_sparse_array_t<shared_client_context_t> _tcp_sockets;
        std::stack<size_t> _unused_tcp_sockets_indexes;
        std::mutex _unused_tcp_sockets_indexes_mut;

        std::atomic_size_t _last_tcp_index;
        std::mutex _tcp_sockets_mut;

        // Events
        std::queue<ServerEvent> _event;
        std::mutex _event_mut;

        boost::atomic_bool _server_running;

        // Threads
        std::unique_ptr<boost::thread> _thread_udp_reader;
        std::unique_ptr<boost::thread> _thread_tcp_reader;
        std::unique_ptr<boost::thread> _thread_tcp_acceptor;

    public:
        void add_event(ServerEvent&& event)
        {
            std::lock_guard<std::mutex> lock(_event_mut);
            _event.push(std::move(event));
        }

    private:
        //
        // Utitlity verbose function for sync_disconnection
        // Is thread safe as long as the good mutex and socket_sparse_array is
        // passed
        //
        template <typename SocketSparseArray>
        void disconnect_any_socket_sync(const size_t index,
            SocketSparseArray& s, std::mutex& mut,
            ServerEvent::ServerEventType event_type)
        {
            std::lock_guard<std::mutex> lock(mut);

            // Maybe handle disconnection failure send event
            if (s.non_null(index)) {
                auto& socket = *s[index].value();
                if (socket.reader_thread && socket.is_reading) {
                    socket.reader_thread->join();
                }
                if (socket.writer_thread && socket.is_writing) {
                    socket.writer_thread->join();
                }
                s.erase(index);

                std::lock_guard<std::mutex> lock_(
                    _unused_tcp_sockets_indexes_mut);
                add_event(std::move(ServerEvent(event_type,
                    std::move(ServerEvent::ServerEventContainer(index)))));
                _unused_tcp_sockets_indexes.push(index);
                if (index == _last_tcp_index - 1)
                    --_last_tcp_index;
            }
        }

        //
        // Utitlity verbose function for sync_connection
        // Is thread safe as long as the good mutex and socket_sparse_array is
        // passed
        //
        template <typename SocketSparseArray>
        void connect_any_socket_sync(SocketSparseArray& s, std::mutex& mut,
            ServerEvent::ServerEventType event_type,
            shared_client_context_t&& client_context)
        {
            std::lock_guard<std::mutex> lock(mut);
            std::lock_guard<std::mutex> lock_(_unused_tcp_sockets_indexes_mut);
            size_t optional_index;

            if (!_unused_tcp_sockets_indexes.empty()) {
                optional_index = _unused_tcp_sockets_indexes.top();
                _unused_tcp_sockets_indexes.pop();
                s.insert(optional_index, std::move(client_context));
                add_event(std::move(ServerEvent(event_type,
                    std::move(
                        ServerEvent::ServerEventContainer(optional_index)))));
            } else {
                s.insert(_last_tcp_index, std::move(client_context));
                add_event(std::move(ServerEvent(event_type,
                    std::move(
                        ServerEvent::ServerEventContainer(_last_tcp_index)))));
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
                    ClientContext* context = new ClientContext(_tcp_io_context);
                    shared_client_context_t socket(context);

                    _tcp_acceptor.accept(socket->socket);

                    // socket->socket.non_blocking(true);

                    connect_any_socket_sync(_tcp_sockets, _tcp_sockets_mut,
                        ServerEvent::ServerEventType::TCP_CONNECTION,
                        std::move(socket));
                } catch (...) {
                }
            } while (_server_running);
        }

        bool read_single_client_tcp_socket_check_index_and_update_client_context(size_t i)
        {
            _tcp_sockets_mut.lock();
            if (!_tcp_sockets.non_null(i)) {
                _tcp_sockets_mut.unlock();
                return false;
            }
            auto& ctx = *_tcp_sockets[i].value();

            if (ctx.should_close) {
                _tcp_sockets_mut.unlock();
                disconnect_tcp_socket_sync(i);
                return false;
            }

            if (ctx.is_reading) {
                _tcp_sockets_mut.unlock();
                return false;
            }
            _tcp_sockets_mut.unlock();
            return true;
        }

        void send_tcp_message_event(ServerEvent::ServerEventContainer&& event_container)
        {
            add_event(std::move(ServerEvent(ServerEvent::ServerEventType::TCP_MESSAGE,
                std::move(event_container))));
        }

        void send_udp_message_event(ServerEvent::ServerEventContainer&& event_container)
        {
            add_event(std::move(ServerEvent(ServerEvent::ServerEventType::UDP_MESSAGE,
                std::move(event_container))));
        }

        void read_single_client_tcp_socket(size_t i)
        {
            if (!read_single_client_tcp_socket_check_index_and_update_client_context(i))
                return;

            auto& ctx = *_tcp_sockets[i].value();
            ctx.is_reading = true;

            if (ctx.reader_thread)
                ctx.reader_thread->join();

            ctx.reader_thread = std::unique_ptr<std::thread>(
                new std::thread([this, i]() {
                    auto& ctx = *_tcp_sockets[i].value();
                    try {
                        size_t readed_bytes = ctx.socket.read_some(boost::asio::buffer(ctx.buffer));

                        if (!readed_bytes)
                            return;
                        send_tcp_message_event(
                            std::move(ServerEvent::ServerEventContainer(
                                ServerEvent::ServerMessageTCP { i, readed_bytes, ctx.buffer }
                        )));
                        ctx.is_reading = false;
                    } catch (...) {
                        ctx.should_close = true;
                    }
                }));
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
            do {
                for (size_t i = 0; i < _last_tcp_index; ++i) {
                    read_single_client_tcp_socket(i);
                }
            } while (_server_running);

            std::lock_guard<std::mutex> lock(_tcp_sockets_mut);

            for (size_t i = 0; i < _tcp_sockets.size(); i++) {
                if (_tcp_sockets.non_null(i)) {
                    auto& ctx = *_tcp_sockets[i].value();
                    if (ctx.reader_thread && ctx.is_reading) {
                        ctx.reader_thread->join();
                    }
                }
            }
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
            udp_buffer_t udp_buffer;
            size_t readed_bytes;

            do {
                boost::asio::ip::udp::endpoint remote_endpoint;
                readed_bytes = _server_udp_socket.receive_from(
                    boost::asio::buffer(udp_buffer), remote_endpoint);

                // TODO: Verify packet integrity and send the good ID (Maybe change ServerMessageUDP struct)
                send_udp_message_event(
                    std::move(ServerEvent::ServerEventContainer(
                        ServerEvent::ServerMessageUDP { 0, readed_bytes, udp_buffer }
                )));
            } while (_server_running);
        }

        //
        // Can be runnned in a separate thread
        // Starts the server if not already running
        // If any problem occured (notably thread problems) returns false
        // If the server is_running() member returns true this function has
        // already been finished (or was already called)
        //
        bool start()
        {
            if (_server_running)
                return true;
            try {
                _server_running = true;
                _thread_tcp_acceptor
                    = std::unique_ptr<boost::thread>(new boost::thread(
                        std::bind(&Server::accept_tcp_clients_sync, this)));

                _thread_tcp_reader
                    = std::unique_ptr<boost::thread>(new boost::thread(
                        std::bind(&Server::read_tcp_sockets, this)));

                _thread_udp_reader
                    = std::unique_ptr<boost::thread>(new boost::thread(
                        std::bind(&Server::read_udp_sockets, this)));
            } catch (...) {
                _server_running = false;
                return false;
            }
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

    public:
        Server(const int tcp_port, const int udp_port)
            : _tcp_acceptor(_tcp_io_context, boost::asio::ip::tcp::endpoint(
                               boost::asio::ip::tcp::v4(), tcp_port))
            , _server_udp_socket(_udp_io_context,
                  boost::asio::ip::udp::endpoint(
                      boost::asio::ip::udp::v4(), udp_port))
        {
            _last_tcp_index = 0;
            _server_running = false;
            start();
            // _tcp_acceptor.listen(RTYPE_SERVER_MAX_CLIENT);
            std::printf("[Server Connected]:\n\t- TCP 127.0.0.1:%d\n\t- UDP 127.0.0.1:%d\n", tcp_port, udp_port);
        }

        ~Server() { stop(); }

    private:
        //
        // can be runned in a separate thread
        // Will add in the event queue the disconnection if the tcp_socket
        // was currently used and will destroy the given socket
        // Only used to disconnect manually a tcp socket for whatever reason
        //
        void disconnect_tcp_socket_sync(const size_t index)
        {
            disconnect_any_socket_sync(index, _tcp_sockets, _tcp_sockets_mut,
                ServerEvent::TCP_DISCONNECTION);
        }

    public:
        //
        // Polls the server for any event that might have occured
        // If no event occured returns false
        // Otherwise it copies the event in the parameter and returns true
        //
        bool poll(ServerEvent& event)
        {
            std::lock_guard<std::mutex> lock(_event_mut);
            if (_event.empty())
                return false;
            event = std::move(_event.front());
            _event.pop();
            return true;
        }

        //
        // Tells whether the server is running
        //
        bool is_running() const { return _server_running; }

        //
        // Write asynchronously to a specific client
        //
        void write_tcp_socket(const size_t index, tcp_buffer_t& tcp_buffer, const size_t size)
        {
            std::lock_guard<std::mutex> lock(_tcp_sockets_mut);

            if (_tcp_sockets.non_null(index)) {
                auto& ctx = *_tcp_sockets[index].value();
                if (ctx.is_writing) {
                    ctx.writer_thread->join();
                }

                tcp_buffer_t *buffer = new tcp_buffer_t(tcp_buffer);

                ctx.is_writing = true;
                ctx.writer_thread = std::unique_ptr<std::thread>(
                    new std::thread([&ctx, &buffer, size]() {
                        ctx.socket.write_some(boost::asio::buffer(buffer, size));
                        delete buffer;
                        ctx.is_writing = false;
                    })
                );
            }
        }

        bool can_write_directly_on_tcp_socket(const size_t index, const void *data)
        {
            std::lock_guard<std::mutex> lock(_tcp_sockets_mut);

            if (_tcp_sockets.non_null(index)) {
                return !_tcp_sockets[index].value()->is_writing;
            }
            return false;
        }
    };

}
}
