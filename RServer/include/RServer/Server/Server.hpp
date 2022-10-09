#pragma once

#include <spdlog/spdlog.h>

#include <variant>

#include "RServer/Messages/Messages.hpp"

#include "RServer/utils.hpp"

#include "PileAA/meta.hpp"

#define MAGIC_NUMBER 0xff1cec0ffeedefec

namespace rtype {
namespace net {

    template <typename BufferType, int StartOffset = 0>
    struct base_message_info {
        BufferType buffer;
        size_t size = 0;

        HL_AUTO_COMPLETE_CANONICAL_FORM(base_message_info);

        base_message_info(BufferType&& buffer, size_t size)
            : buffer(std::move(buffer))
            , size(size)
        {
        }

        template <typename To> To to()
        {
            return To(this->buffer.c_array() + StartOffset,
                this->buffer.c_array() + StartOffset + this->size);
        }

        std::string to_string()
        {
            return to<std::string>();
        }

        std::vector<uint8_t> to_vec()
        {
            return to<std::vector<uint8_t>>();
        }

        boost::shared_ptr<IMessage> to_msg()
        {
            return parse_message(to<std::vector<uint8_t>>());
        }
        template <typename T> std::shared_ptr<T> to_msg()
        {
            return parse_message<T>(to<std::vector<uint8_t>>());
        }

        message_code code()
        {
            if (buffer.size() > 0)
                return static_cast<message_code>(buffer[0]);
            return message_code::DUMMY;
        }
    };

    class tcp_connection
        : public boost::enable_shared_from_this<tcp_connection> {
    public:
        using pointer = boost::shared_ptr<tcp_connection>;

        struct message_info : public base_message_info<tcp_buffer_t> {
            HL_AUTO_COMPLETE_CANONICAL_FORM(message_info);

            message_info(tcp_buffer_t&& buffer, size_t size)
                : base_message_info(std::move(buffer), size)
            {
            }
        };

        using shared_message_info_t = boost::shared_ptr<message_info>;

        static shared_message_info_t new_message(const void* data, size_t size);

        static shared_message_info_t new_message(const IMessage& msg);


        static shared_message_info_t new_message(const std::string& s);

        void start();

        static pointer create(boost::asio::io_context& io_context);

        tcp::socket& socket();

        void send(shared_message_info_t message);


        bool poll(shared_message_info_t& message);


        bool should_exit() const;

    private:
        void handle_read();

        tcp_connection(boost::asio::io_context& io_context);

    public:
        ~tcp_connection() = default;
        void set_id(size_t id) { _id = id; }
        size_t get_id() const { return _id; }

    private:
        tcp::socket _socket;

        boost::shared_ptr<async_automated_sparse_array<message_info>>
            _send_message_list;
        boost::shared_ptr<std::atomic_bool> _should_exit;
        boost::shared_ptr<tcp_buffer_t> _buffer_reader;
        boost::shared_ptr<async_queue<shared_message_info_t>>
            _readed_messages_queue;

        size_t _id = -1;
    };

    class tcp_event_connexion {
    private:
        size_t _id = -1;

    public:
        tcp_event_connexion(size_t id);

        size_t get_id() const;

        HL_AUTO_COMPLETE_CANONICAL_FORM(tcp_event_connexion);
    };

    class tcp_event_disconnexion {
    private:
        size_t _id = -1;

    public:
        tcp_event_disconnexion(size_t id);

        size_t get_id() const;

        HL_AUTO_COMPLETE_CANONICAL_FORM(tcp_event_disconnexion);
    };

    class tcp_event_message {
    private:
        size_t _id = -1;
        tcp_connection::shared_message_info_t _message;

    public:
        tcp_event_message(
            size_t id, tcp_connection::shared_message_info_t message);

        size_t get_id() const;
        tcp_connection::shared_message_info_t get_message();

        HL_AUTO_COMPLETE_CANONICAL_FORM(tcp_event_message);
    };

    enum tcp_event_type { Invalid, Connexion, Disconnexion, Message };

    using tcp_event_container = std::variant<void*, tcp_event_connexion,
        tcp_event_disconnexion, tcp_event_message>;

    class tcp_event {
    private:
        tcp_event_container _container;

    public:
        tcp_event();

        tcp_event(tcp_event_connexion event);
        tcp_event(tcp_event_disconnexion event);
        tcp_event(tcp_event_message event);

        ~tcp_event() = default;

        template <typename T> const T& get() const
        {
            return std::get<T>(_container);
        }

        template <typename T> T& get() { return std::get<T>(_container); }

        int get_type() const { return _container.index(); }
    };

    class tcp_server {
    public:
        tcp_server(boost::asio::io_context& io_context, int port);


        ~tcp_server();

        bool poll(tcp_event& event);

        void send(size_t id, tcp_connection::shared_message_info_t message);

    private:
        void start_accept();

        void handle_accept(tcp_connection::pointer new_connection,
            const boost::system::error_code& error);


        void poll_tcp_connections();

        boost::asio::io_context& _io_context;
        tcp::acceptor _acceptor;
        async_automated_sparse_array<tcp_connection> _connections;
        std::atomic<bool> _is_started = false;

        std::unique_ptr<boost::thread> _tcp_connection_polling_thread;

        async_queue<tcp_event> _events;
    };

    class udp_server {
    public:
        class message_info : public base_message_info<udp_buffer_t,
                                 2 * sizeof(uint64_t) + sizeof(uint16_t)> {
        public:
            message_info(
                udp::endpoint sender, udp_buffer_t&& buffer, size_t size);

            char *msg() const;

            void set_msg(char *msg);

            size_t size();

            void set_size(size_t size);

            udp::endpoint sender();

            uint64_t seq_num();

            uint16_t sender_id();

            HL_AUTO_COMPLETE_CANONICAL_FORM(message_info);

        private:
            char *_msg;
            size_t _size;
            udp::endpoint _sender;
            uint16_t _sender_id;
            uint16_t _seq_num;
        };

        using shared_message_info_t = boost::shared_ptr<message_info>;

        static shared_message_info_t new_message(
            int sender, const void* data, size_t size);

        static shared_message_info_t new_message(
            int sender, const IMessage& msg);

        static shared_message_info_t new_message(
            int sender, const std::string& s);

    private:
        void handle_receive_from(const boost::system::error_code& error,
            std::size_t bytes_transferred);

        void start_receive();

    public:

        bool poll(shared_message_info_t& info);

        void send_to(udp::endpoint target, shared_message_info_t message);

        udp_server(boost::asio::io_context& io_context, int port);

    private:
        udp::socket _socket;
        udp::endpoint _sender_endpoint;

        boost::shared_ptr<udp_buffer_t> _recv_buffer;
        boost::shared_ptr<async_automated_sparse_array<message_info>> _messages;
        boost::shared_ptr<async_queue<shared_message_info_t>> _recv_queue;
    };

    class remote_client : public boost::enable_shared_from_this<remote_client> {
    public:
        using pointer = boost::shared_ptr<remote_client>;

        static pointer create() { return pointer(new remote_client()); }

        remote_client();

        void init_main_channel(tcp_server& main_channel, size_t main_id);

        void init_feed_channel(
            udp_server& feed_channel, udp::endpoint feed_endpoint);

        using shared_main_message_info_t
            = tcp_connection::shared_message_info_t;
        using main_message_info_t = tcp_connection::message_info;

        using shared_feed_message_info_t = udp_server::shared_message_info_t;
        using feed_message_info_t = udp_server::message_info;

        void send_main(tcp_connection::shared_message_info_t msg);

        void send_main(const std::string& s);

        void send_main(const void* data, size_t size);

        void send_main(const rtype::net::IMessage& message);

        void send_main(rtype::net::IMessage& message);

        void send_feed(udp_server::shared_message_info_t msg);

        void send_feed(const std::string& s);

        void send_feed(const void* data, size_t size);

        void send_feed(const rtype::net::IMessage& message);

        void send_feed(rtype::net::IMessage& message);

        int id() const;

        udp::endpoint get_feed_endpoint() const;

    private:
        size_t _main_id;
        udp::endpoint _feed_endpoint;
        tcp_server* _main_channel;
        udp_server* _feed_channel;
    };
    class server {
    public:
        server(int tcp_port, int udp_port, bool authenticate = false);
        ~server() { _is_running = false; _thread_io_context_runner->join(); }

        enum event_type {
            Invalid,
            MainMessage,
            FeedMessage,
            Connect,
            Disconnect
        };

        class base_message {
        public:
            virtual ~base_message() = default;
            virtual remote_client::pointer sender() const = 0;
            virtual std::string to_string() const = 0;
            virtual std::vector<uint8_t> to_vec() const = 0;
            virtual boost::shared_ptr<IMessage> to_msg() = 0;
            virtual message_code code() const = 0;
        };

        class main_message : public base_message {
        public:
            // main_message(remote_client::pointer sender,
            //     tcp_connection::shared_message_info_t msg);

            main_message(remote_client::pointer sender,
                        tcp_connection::shared_message_info_t msg);

            remote_client::pointer sender() const override;
            std::string to_string() const override;
            std::vector<uint8_t> to_vec() const override;
            boost::shared_ptr<IMessage> to_msg() override;

            message_code code() const override;

        private:
            remote_client::pointer _sender;
            tcp_connection::shared_message_info_t _msg;
        };

        class feed_message : public base_message {
        public:
            feed_message(remote_client::pointer sender,
                udp_server::shared_message_info_t msg);

            remote_client::pointer sender() const override;
            std::string to_string() const override;
            std::vector<uint8_t> to_vec() const override;
            boost::shared_ptr<IMessage> to_msg() override;
            message_code code() const override;

        private:
            remote_client::pointer _sender;
            udp_server::shared_message_info_t _msg;
        };

        struct event {
            event_type type;
            remote_client::pointer client;
            std::unique_ptr<server::base_message> message;

            event()
                : type(Invalid)
                , message(nullptr)
                , client(nullptr)
            {
            }
        };

        bool poll(event& event);

        boost::asio::io_context& io_context();

        tcp_server& tcp();
        udp_server& udp();
        std::unordered_map<uint16_t, remote_client::pointer>& clients();

    private:
        void run();

        bool poll_tcp(event& event, tcp_event& tcp_event);
        bool on_tcp_event_connexion(event& event, tcp_event& tcp_event);
        bool on_tcp_event_disconnexion(event& event, tcp_event& tcp_event);
        bool on_tcp_event_message(event& event, tcp_event& tcp_event);

        bool poll_udp(event& event, udp_server::shared_message_info_t& msg);
        bool on_udp_feed_init(event& event, udp_server::shared_message_info_t& msg);
        bool on_udp_event_message(event& event, udp_server::shared_message_info_t& msg);

    public:
        remote_client::pointer get_client(uint16_t id);

        remote_client::pointer get_client(tcp_connection::pointer conn);
        remote_client::pointer get_client(const udp::endpoint& endpoint);

    private:
        bool _authenticate = false;
        boost::asio::io_context _io_context;

        boost::shared_ptr<tcp_server> _tcp_server;
        boost::shared_ptr<udp_server> _udp_server;

        std::unordered_map<uint16_t, remote_client::pointer> _clients;

        boost::shared_ptr<boost::thread> _thread_io_context_runner;

        std::atomic_size_t _is_running = false;
    };
}
}
