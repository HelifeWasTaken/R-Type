#include "RServer/Server/Server.hpp"
#include <vector>
#include <assert.h>
#include <boost/endian.hpp>
#include <iostream>

namespace rtype {
namespace net {


    tcp_connection::shared_message_info_t tcp_connection::new_message(const void* data, BufferSizeType size)
    {
        assert(size < tcp_buffer_t::size());
        message_info* mesg = new message_info;
        std::memcpy(mesg->buffer.c_array(), data, size);
        mesg->size = size;
        return shared_message_info_t(mesg);
    }

    tcp_connection::shared_message_info_t tcp_connection::new_message(const IMessage& msg)
    {
        auto buffer = msg.serialize();
        return new_message(buffer.data(), buffer.size());
    }

    tcp_connection::shared_message_info_t tcp_connection::new_message(const std::string& s)
    {
        return new_message(
            reinterpret_cast<const void*>(s.c_str()), s.size()
        );
    }

    void tcp_connection::start()
    {
        handle_read();
    }

    tcp_connection::pointer tcp_connection::create(boost::asio::io_context& io_context)
    {
        return tcp_connection::pointer(new tcp_connection(io_context));
    }

    tcp::socket& tcp_connection::socket()
    {
        return _socket;
    }

    void tcp_connection::send(tcp_connection::shared_message_info_t message)
        {
            spdlog::info(
                "tcp_connection({}): Starting to send a message!", _id);
            BufferSizeType index = _send_message_list->async_set(message);
            _socket.async_send(
                boost::asio::buffer(message->buffer, message->size),
                [should_exit = _should_exit,
                    send_message_list = _send_message_list, index, id = _id](
                    boost::system::error_code ec, BufferSizeType sended_bytes) {
                    // TODO: Maybe check sended bytes
                    (void)sended_bytes;
                    if (ec) {
                        // TODO: Maybe check error type
                        spdlog::error("tcp_connection({}): Error while sending "
                                      "message({}): {}",
                            id, index, ec.message());
                    } else {
                        spdlog::info("tcp_connection({}): Sucessfully sended "
                                     "message({})",
                            id, index);
                        send_message_list->async_remove(index);
                    }
                });
        }

    bool tcp_connection::poll(tcp_connection::shared_message_info_t& message)
    {
        return _readed_messages_queue->async_pop(message);
    }

    bool tcp_connection::should_exit() const
    {
        return *_should_exit;
    }

    void tcp_connection::handle_read()
    {
        spdlog::info(
            "tcp_connection({}): Starting to read a message!", _id);

        _socket.async_receive(boost::asio::buffer(*_buffer_reader),
            [this, should_exit = _should_exit,
                readed_messages_queue = _readed_messages_queue,
                buffer_reader = _buffer_reader,
                id = _id](const boost::system::error_code& error,
                BufferSizeType bytes_transferred) {
                if (error) {
                    // TODO: Maybe check error type
                    spdlog::error("tcp_connection({}): Error while reading "
                                    "from socket: {}",
                        id, error.message());
                    *should_exit = true;
                } else {
                    if (bytes_transferred) {
                        readed_messages_queue->async_push(
                            shared_message_info_t(
                                new tcp_connection::message_info(std::move(*buffer_reader),
                                    bytes_transferred)));
                        spdlog::info("tcp_connection({}): Added to message "
                                        "queue a new message",
                            id);
                    }
                    if (!(*should_exit))
                        handle_read();
                }
            }
        );
    }

    tcp_connection::tcp_connection(boost::asio::io_context& io_context)
                                    : _socket(io_context)
                                    , _send_message_list(new async_automated_sparse_array<tcp_connection::message_info>)
                                    , _should_exit(new std::atomic_bool)
                                    , _buffer_reader(new tcp_buffer_t)
                                    , _readed_messages_queue(new async_queue<shared_message_info_t>)
    {
        *_should_exit = false;
    }

    tcp_event_connexion::tcp_event_connexion(ClientID id) : _id(id)
    {

    }

    ClientID tcp_event_connexion::get_id() const
    { return _id; }

    tcp_event_disconnexion::tcp_event_disconnexion(ClientID id) : _id(id)
    {
    }

    ClientID tcp_event_disconnexion::get_id() const
    { return _id; }

    tcp_event_message::tcp_event_message(ClientID id,
                    tcp_connection::shared_message_info_t message)
                    : _id(id)
                    , _message(message)
    {
    }

    ClientID tcp_event_message::get_id() const
    { return _id; }

    tcp_connection::shared_message_info_t tcp_event_message::get_message()
    { return _message; }

    tcp_event::tcp_event() : _container(nullptr)
    {}

    tcp_event::tcp_event(tcp_event_connexion event) : _container(event)
    {}

    tcp_event::tcp_event(tcp_event_message event)
                    : _container(event)
    {}

    tcp_event::tcp_event(tcp_event_disconnexion event) : _container(event)
    {}

    tcp_server::tcp_server(boost::asio::io_context& io_context, PortType port)
                        : _io_context(io_context)
                        , _acceptor(io_context, tcp::endpoint(tcp::v4(), port))
    {
        spdlog::info("tcp_server: launched: 127.0.0.1:{}", port);
        _is_started = true;
        _acceptor.listen(RTYPE_MAX_SERVER_CONNECTIONS);
        start_accept();
        poll_tcp_connections();
    }

    tcp_server::~tcp_server()
    {
        _is_started = false;
        if (_tcp_connection_polling_thread)
            _tcp_connection_polling_thread->join();
    }

    bool tcp_server::poll(tcp_event& event)
    {
        return _events.async_pop(event);
    }

    void tcp_server::send(ClientID id, tcp_connection::shared_message_info_t message)
    {
        auto it = _connections.async_get(id);
        spdlog::info("tcp_server: Trying to send a message to {}", id);
        if (it)
            it->send(message);
        else
            spdlog::error("tcp_server: Error while sending message: "
                            "connection {} not found",
                id);
    }

    void tcp_server::start_accept()
    {
        tcp_connection::pointer new_connection
            = tcp_connection::create(_io_context);

        spdlog::info("tcp_server: Starting to accept a new connection");
        _acceptor.async_accept(new_connection->socket(),
            boost::bind(&tcp_server::handle_accept, this, new_connection,
                boost::asio::placeholders::error));
    }

    void tcp_server::handle_accept(tcp_connection::pointer new_connection,
        const boost::system::error_code& error)
    {
        if (!error) {
            BufferSizeType id = _connections.async_set(new_connection);
            new_connection->set_id(id);
            new_connection->start();
            _events.async_push(tcp_event_connexion(id));
            spdlog::info("tcp_server: New connection accepted: {}", id);
        } else {
            spdlog::error("tcp_server: Accept Error: {}", error.message());
            return;
        }
        start_accept();
    }

    void tcp_server::poll_tcp_connections()
    {
        spdlog::info("tcp_server: Starting to poll tcp connections");
        _tcp_connection_polling_thread = std::unique_ptr<boost::thread>(
            new boost::thread([this]() {
                while (_is_started) {
                    tcp_connection::shared_message_info_t message;
                    for (BufferSizeType i = 0; i < _connections.async_size(); ++i) {
                        auto connection = _connections.async_get(i);
                        if (!connection) {
                            continue;
                        }
                        if (connection->should_exit()) {
                            _events.async_push(
                                std::move(tcp_event_disconnexion(i)));
                            _connections.async_remove(i);
                            spdlog::info(
                                "tcp_server: Disconnection from {}", i);
                        } else {
                            while (connection->poll(message)) {
                                _events.async_push(std::move(
                                    tcp_event_message(i, message)));
                                spdlog::info(
                                    "tcp_server: Polled a message from {}",
                                    i);
                            }
                        }
                    }
                }
            })
        );
    }

    udp_server::message_info::message_info(
                udp::endpoint sender, udp_buffer_t&& buffer, BufferSizeType size)
                : base_message_info(std::move(buffer), size)
                , _sender(sender)
    {
        Byte* pos = this->buffer.c_array();
        Serializer serializer(pos, size);

        serializer >> _magic >> _sender_id;
        _size = buffer.size();
    }

    Byte *udp_server::message_info::msg() const { return _msg; }

    void udp_server::message_info::set_msg(Byte *msg) { _msg = msg; }

    BufferSizeType udp_server::message_info::size() { return _size; }

    void udp_server::message_info::set_size(BufferSizeType size) { _size = size; }

    udp::endpoint udp_server::message_info::sender() { return _sender; }

    ClientID udp_server::message_info::sender_id() { return _sender_id; }

    udp_server::shared_message_info_t udp_server::new_message(
            ClientID sender, const void* data, BufferSizeType size)
    {
        static const MagicNumber magic = RTYPE_MAGIC_NUMBER;

        Serializer s;

        s << magic << sender;
        s.add_bytes(reinterpret_cast<const Byte*>(data), size);

        assert(s.data.size() < udp_buffer_t::size() - RTYPE_UDP_MESSAGE_HEADER);

        udp_server::shared_message_info_t mesg(new message_info);

        std::memcpy(mesg->buffer.c_array(), s.data.data(), s.data.size());

        mesg->set_msg(mesg->buffer.c_array());
        mesg->set_size(s.data.size());

        return mesg;
    }

    udp_server::shared_message_info_t udp_server::new_message(
        ClientID sender, const IMessage& msg)
    {
        auto buffer = msg.serialize();
        return new_message(sender, buffer.data(), buffer.size());
    }

    udp_server::shared_message_info_t udp_server::new_message(
        ClientID sender, const std::string& s)
    {
        return new_message(
            sender, reinterpret_cast<const void*>(s.c_str()), s.size());
    }

    void udp_server::handle_receive_from(const boost::system::error_code& error,
                                    BufferSizeType bytes_transferred)
    {
        if (!error && bytes_transferred > 0) {
            _recv_queue->async_push(
                shared_message_info_t(new message_info(_sender_endpoint,
                    std::move(*_recv_buffer), bytes_transferred)));
        } else {
            if (error) {
                spdlog::error("udp_server: Cannot read: error {}: {}",
                    error.value(), error.message());
            }
        }
        start_receive();
    }

    void udp_server::start_receive()
    {
        _socket.async_receive_from(boost::asio::buffer(*_recv_buffer),
            _sender_endpoint,
            boost::bind(&udp_server::handle_receive_from, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    bool udp_server::poll(shared_message_info_t& info)
    {
        return _recv_queue->async_pop(info);
    }

    void udp_server::send_to(udp::endpoint target, shared_message_info_t message)
    {
        BufferSizeType index = _messages->async_set(message);

        spdlog::info("udp_server: Sending bytes({}) to {}:{}", message->size(),
            target.address().to_string(), target.port());
        _socket.async_send_to(
            boost::asio::buffer(message->buffer, message->size()), target,
            [index, buffer_copy = message, messages = _messages](
                const boost::system::error_code& error,
                BufferSizeType bytes_transferred) {
                (void)bytes_transferred;
                if (!error) {
                    spdlog::info("udp_server: Successfully seneded a message of size({})", buffer_copy->size());
                } else {
                    spdlog::error("udp_server: Cannot write: error {}: {}",
                        error.value(), error.message());
                }
                messages->async_remove(index);
            }
        );
    }

    udp_server::udp_server(boost::asio::io_context& io_context, PortType port)
        : _socket(io_context, udp::endpoint(udp::v4(), port))
        , _recv_buffer(new udp_buffer_t)
        , _messages(new async_automated_sparse_array<message_info>)
        , _recv_queue(new async_queue<shared_message_info_t>)
    {
        spdlog::info("udp_server: launched: 127.0.0.1:{}", port);
        start_receive();
    }

    remote_client::remote_client()
        : _main_channel(nullptr)
        , _feed_channel(nullptr)
        , _main_id(0)
    {
    }

    void remote_client::init_main_channel(tcp_server& main_channel, ClientID main_id)
    {
        _main_channel = &main_channel;
        _main_id = main_id;
        spdlog::info("remote_client: Main channel({}) initialized", _main_id);
    }

    void remote_client::init_feed_channel(
        udp_server& feed_channel, udp::endpoint feed_endpoint)
    {
        _feed_channel = &feed_channel;
        _feed_endpoint = feed_endpoint;
    }

    void remote_client::send_main(tcp_connection::shared_message_info_t msg)
    {
        if (_main_channel) {
            spdlog::info("remote_client: Sending message to main channel");
            _main_channel->send(_main_id, std::move(msg));
        } else {
            spdlog::error("remote_client: Cannot send to main channel: no "
                            "connection");
        }
    }

    void remote_client::send_main(const std::string& s)
    {
        send_main(tcp_connection::new_message(s));
    }

    void remote_client::send_main(const void* data, BufferSizeType size)
    {
        send_main(tcp_connection::new_message(data, size));
    }

    void remote_client::send_main(const rtype::net::IMessage& message)
    {
        auto bytes = message.serialize();
        send_main(bytes.data(), bytes.size());
    }

    void remote_client::send_main(rtype::net::IMessage& message)
    {
        auto bytes = message.serialize();
        send_main(bytes.data(), bytes.size());
    }

    void remote_client::send_feed(udp_server::shared_message_info_t msg)
    {
        if (_feed_channel) {
            spdlog::info("remote_client: Sending feed message");
            _feed_channel->send_to(_feed_endpoint, std::move(msg));
        } else {
            spdlog::error("remote_client: Cannot send to feed channel: no "
                            "connection");
        }
    }

    void remote_client::send_feed(const std::string& s)
    {
        send_feed(udp_server::new_message(_main_id, s));
    }

    void remote_client::send_feed(const void* data, BufferSizeType size)
    {
        send_feed(udp_server::new_message(_main_id, data, size));
    }

    void remote_client::send_feed(const rtype::net::IMessage& message)
    {
        auto bytes = message.serialize();
        send_feed(bytes.data(), bytes.size());
    }

    void remote_client::send_feed(rtype::net::IMessage& message)
    {
        auto bytes = message.serialize();
        send_feed(bytes.data(), bytes.size());
    }

    ClientID remote_client::id() const { return _main_id; }

    udp::endpoint remote_client::get_feed_endpoint() const { return _feed_endpoint; }

    server::server(PortType tcp_port, PortType udp_port, Bool authenticate)
            : _io_context(boost::asio::io_context())
            , _tcp_server(new tcp_server(_io_context, tcp_port))
            , _udp_server(new udp_server(_io_context, udp_port))
            , _authenticate(authenticate)
    {
        _is_running = true;
        run();
    }

    server::main_message::main_message(remote_client::pointer sender,
        tcp_connection::shared_message_info_t msg)
        : _sender(sender)
        , _msg(msg)
    {
    }

    remote_client::pointer server::main_message::sender() const { return _sender; }
    std::string server::main_message::to_string() const { return _msg->to_string(); }
    std::vector<Byte> server::main_message::to_vec() const { return _msg->to_vec(); }
    boost::shared_ptr<IMessage> server::main_message::to_msg() { return _msg->to_msg(); }

    message_code server::main_message::code() const { return _msg->code(); }

    server::feed_message::feed_message(remote_client::pointer sender,
                udp_server::shared_message_info_t msg)
                : _sender(sender)
                , _msg(msg)
    {
    }

    remote_client::pointer server::feed_message::sender() const { return _sender; }
    std::string server::feed_message::to_string() const { return _msg->to_string(); }
    std::vector<Byte> server::feed_message::to_vec() const
    {
        return _msg->to_vec();
    }

    boost::shared_ptr<IMessage> server::feed_message::to_msg()
    {
        return _msg->to_msg();
    }

    message_code server::feed_message::code() const { return _msg->code(); }

    bool server::on_tcp_event_connexion(event& event, tcp_event& tcp_event)
    {
        auto conn = tcp_event.get<tcp_event_connexion>();
        spdlog::info("server: on_tcp_connection: New client connected: {}", conn.get_id());

        event.type = rtype::net::server::event_type::Connect;
        event.client = _clients.insert_or_assign(conn.get_id(), remote_client::create()).first->second;
        event.client->init_main_channel(*_tcp_server, conn.get_id());
        return true;
    }

    bool server::on_tcp_event_disconnexion(event& event, tcp_event& tcp_event)
    {
        auto disconn = tcp_event.get<tcp_event_disconnexion>();
        spdlog::info("server: on_tcp_disconnection: Client disconnected: {}", disconn.get_id());

        event.type = rtype::net::server::event_type::Disconnect;
        event.client = get_client(disconn.get_id());
        return true;
    }

    bool server::on_tcp_event_message(event& event, tcp_event& tcp_event)
    {
        event.type = rtype::net::server::event_type::MainMessage;
        {
            tcp_event_message& msg_event = tcp_event.get<tcp_event_message>();
            event.client = get_client(msg_event.get_id());
            event.message = std::make_unique<main_message>(event.client, msg_event.get_message());
        }
        if (_authenticate) {
            if (event.message->code() == message_code::CONN_INIT) {
                spdlog::info("server: on_tcp_message: ConnectionInitReply sent to client: {}", event.client->id());
                event.client->send_main(ConnectionInitReply(event.client->id(), RTYPE_CONN_INIT_REPLY_TOKEN));
            } else {
                spdlog::info("server: on_tcp_message: New message from client: {}", event.client->id());
            }
        }
        return true;
    }

    bool server::poll_tcp(event& event, tcp_event& tcp_event)
    {
        switch (tcp_event.get_type()) {
        case tcp_event_type::Connexion:
            return on_tcp_event_connexion(event, tcp_event);
        case tcp_event_type::Disconnexion:
            return on_tcp_event_disconnexion(event, tcp_event);
        case tcp_event_type::Message:
            return on_tcp_event_message(event, tcp_event);
        default:
            spdlog::error("server: poll_tcp: Invalid event type");
            event.type = rtype::net::server::event_type::Invalid;
            return false;
        }
        return true;
    }

    bool server::on_udp_event_message(event& event, udp_server::shared_message_info_t& msg)
    {
        spdlog::info("server: on_udp_message: New message from client: {} of code {}", msg->sender_id(), (int8_t)msg->code());
        event.type = rtype::net::server::event_type::FeedMessage;
        event.client = get_client(msg->sender_id());
        event.message = std::make_unique<feed_message>(event.client, msg);
        return true;
    }

    bool server::on_udp_feed_init(event& event, udp_server::shared_message_info_t& msg)
    {
        spdlog::info("server: on_udp_feed_init: New feed from client: {}", msg->sender_id());
        event.client = get_client(msg->sender_id());
        event.client->init_feed_channel(*_udp_server, msg->sender());
        event.client->send_feed(FeedInitReply(RTYPE_FEED_INIT_TOKEN));
        return false;
    }

    bool server::poll_udp(event& event, udp_server::shared_message_info_t& msg)
    {
        if (_authenticate && msg->code() == message_code::FEED_INIT) {
            return on_udp_feed_init(event, msg);
        } else {
            return on_udp_event_message(event, msg);
        }
    }

    bool server::poll(event& event)
    {
        {
            tcp_event tcp_event;
            if (_tcp_server->poll(tcp_event)) {
                return poll_tcp(event, tcp_event);
            }
        }

        {
            udp_server::shared_message_info_t msg;
            if (_udp_server->poll(msg)) {
                return poll_udp(event, msg);
            }
        }
        return false;
    }

    boost::asio::io_context& server::io_context() { return _io_context; }

    tcp_server& server::tcp() { return *_tcp_server; }
    udp_server& server::udp() { return *_udp_server; }

    std::unordered_map<ClientID, remote_client::pointer>& server::clients()
    {
        return _clients;
    }

    void server::run()
    {
        _thread_io_context_runner = boost::shared_ptr<boost::thread>(
            new boost::thread([&is_running=_is_running, &io = _io_context]() {
                while (is_running)
                    io.run();
            }));
    }

    remote_client::pointer server::get_client(ClientID id)
    {
        auto c = _clients.find(id);
        if (c != _clients.end()) {
            return c->second;
        }
        throw std::runtime_error("remote_client: Cannot find client");
    }

    remote_client::pointer server::get_client(tcp_connection::pointer conn)
    {
        auto it = std::find_if(
            _clients.begin(), _clients.end(), [conn](const auto& c) {
                return c.second->id() == conn->get_id();
            });
        if (it != _clients.end()) {
            return it->second.get()->shared_from_this();
        } else {
            throw std::runtime_error("remote_client: Cannot find client");
        }
    }

    remote_client::pointer server::get_client(const udp::endpoint& endpoint)
    {
        auto it = std::find_if(
            _clients.begin(), _clients.end(), [endpoint](const auto& c) {
                return c.second->get_feed_endpoint() == endpoint;
            });
        if (it != _clients.end()) {
            return it->second.get()->shared_from_this();
        } else {
            throw std::runtime_error("remote_client: Cannot find client");
        }
    }
}
}
