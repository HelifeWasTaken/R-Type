#pragma once

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>
#include <queue>
#include <variant>
#include "include/PileAA/meta.hpp"

// TODO: Use proper logging library vs std::fprintf stderr

namespace rtype {
    namespace net {

        template<typename T>
        class async_queue : public std::queue<T> {
        public:
            HL_AUTO_COMPLETE_CANONICAL_FORM(async_queue);

            bool async_empty() {
                std::lock_guard<std::mutex> lock(_mutex);
                return this->empty();
            }

            bool async_pop(T& value) {
                std::lock_guard<std::mutex> lock(_mutex);
                if (this->empty())
                    return false;
                value = std::move(this->front());
                this->pop();
                return true;
            }

            void async_push(const T& value) {
                std::lock_guard<std::mutex> lock(_mutex);
                this->push(value);
            }

            void async_push(T&& value) {
                std::lock_guard<std::mutex> lock(_mutex);
                this->push(std::move(value));
            }

            template<typename ...Args>
            void async_emplace(Args&&... args) {
                std::lock_guard<std::mutex> lock(_mutex);
                this->emplace(std::forward<Args>(args)...);
            }

        private:
            std::mutex _mutex;
        };

        template<typename T>
        class async_automated_sparse_array {
        private:
            std::mutex _mut;
            async_queue<size_t> _unused_indexes;
            std::vector<boost::shared_ptr<T>> _array;

        public:
            HL_AUTO_COMPLETE_CANONICAL_FORM(async_automated_sparse_array);

            boost::shared_ptr<T> async_get(size_t index) {
                std::lock_guard<std::mutex> lock(this->_mut);
                if (index >= this->size())
                    return nullptr;
                return this->_array.at(index);
            }

            size_t async_set(boost::shared_ptr<T> value) {
                std::lock_guard<std::mutex> lock(this->_mut);
                size_t index;

                if (this->_unused_indexes.async_pop(index)) {
                    this->_array.at(index) = value;
                    return index;
                }
                this->_array.push_back(value);
                return this->_array.size() - 1;
            }

            void async_remove(size_t index) {
                std::lock_guard<std::mutex> lock(_mut);
                if (index >= this->_array.size())
                    return;
                this->_array.at(index) = nullptr;
                this->_unused_indexes.async_push(index);
            }

            // TODO: Optimize size by avoiding unused indexes
            size_t size() {
                std::lock_guard<std::mutex> lock(this->_mut);
                return this->_array.size();
            }
        };

#ifndef RTYPE_TCP_BUFFER_SIZE
#define RTYPE_TCP_BUFFER_SIZE 1024
#endif

#ifndef RTYPE_UDP_BUFFER_SIZE
#define RTYPE_UDP_BUFFER_SIZE 1024
#endif

        using tcp = boost::asio::ip::tcp;
        using udp = boost::asio::ip::tcp;

        using tcp_buffer_t = boost::array<char, RTYPE_TCP_BUFFER_SIZE>;
        using udp_buffer_t = boost::array<char, RTYPE_UDP_BUFFER_SIZE>;

        template<typename BufferType>
        struct base_message_info {
            BufferType buffer;
            size_t size = 0;

            HL_AUTO_COMPLETE_CANONICAL_FORM(base_message_info);

            base_message_info(BufferType&& buffer, size_t size)
                : buffer(std::move(buffer))
                , size(size)
            {}
        };

        class tcp_connection
            : public boost::enable_shared_from_this<tcp_connection>
        {
            public:
                using pointer = boost::shared_ptr<tcp_connection>;

                struct message_info : public base_message_info<tcp_buffer_t> {
                    HL_AUTO_COMPLETE_CANONICAL_FORM(message_info);

                    message_info(tcp_buffer_t&& buffer, size_t size)
                        : base_message_info(std::move(buffer), size)
                    {}
                };

                using shared_message_info_t = boost::shared_ptr<message_info>;

                void start()
                {
                    handle_read();
                }

                static pointer create(boost::asio::io_context& io_context)
                {
                    return pointer(new tcp_connection(io_context));
                }

                tcp::socket& socket()
                {
                    return _socket;
                }

                void send(shared_message_info_t message)
                {
                    size_t index = _send_message_list.async_set(message);
                    _socket.async_send(boost::asio::buffer(message->buffer, message->size),
                            [this, index](boost::system::error_code ec, std::size_t sended_bytes) {
                                // TODO: Maybe check sended bytes
                                (void)sended_bytes;
                                if (ec) {
                                    // TODO: Maybe check error type
                                    std::fprintf(stderr, "Error while sending message: %s", ec.message().c_str());
                                    _should_exit = true;
                                } else {
                                    _send_message_list.async_remove(index);
                                }
                            });
                }

                bool poll(shared_message_info_t& message)
                {
                    return _readed_messages_queue.async_pop(message);
                }

                bool should_exit() const { return _should_exit; }

            private:
                void handle_read()
                {
                    _socket.async_receive(
                        boost::asio::buffer(this->_buffer_reader),
                        _buffer_reader.size(),
                        [this](const boost::system::error_code& error, size_t bytes_transferred) {
                            if (error) {
                                // TODO: Maybe check error type
                                std::fprintf(stderr, "Error while reading from socket: %s", error.message().c_str());
                                _should_exit = true;
                            } else {
                                if (bytes_transferred) {
                                    _readed_messages_queue.async_push(
                                            shared_message_info_t(
                                                new message_info(std::move(_buffer_reader), bytes_transferred)
                                            )
                                        );
                                }
                                handle_read();
                            }
                        }
                    );
                }

                tcp_connection(boost::asio::io_context& io_context)
                    : _socket(io_context)
                {
                    _should_exit = false;
                }

            public:
                ~tcp_connection() = default;

            private:
                tcp::socket _socket;
                async_automated_sparse_array<message_info> _send_message_list;
                tcp_buffer_t _buffer_reader;
                async_queue<shared_message_info_t> _readed_messages_queue;
                std::atomic_bool _should_exit;
        };

        class tcp_event_connexion {
        private:
            size_t _id = -1;

        public:
            tcp_event_connexion(size_t id) : _id(id) {}
            size_t getId() const { return _id; }

            HL_AUTO_COMPLETE_CANONICAL_FORM(tcp_event_connexion);
        };

        class tcp_event_disconnexion {
        private:
            size_t _id = -1;
        public:
            tcp_event_disconnexion(size_t id) : _id(id) {}
            size_t getId() const { return _id; }

            HL_AUTO_COMPLETE_CANONICAL_FORM(tcp_event_disconnexion);
        };

        class tcp_event_message {
        private:
            size_t _id = -1;
            tcp_connection::shared_message_info_t _message;
        public:
            tcp_event_message(size_t id, tcp_connection::shared_message_info_t message)
                : _id(id), _message(message) {}

            size_t getId() const { return _id; }
            tcp_connection::shared_message_info_t getMessage() { return _message; }

            HL_AUTO_COMPLETE_CANONICAL_FORM(tcp_event_message);
        };

        enum tcp_event_type {
            Connexion,
            Disconnexion,
            Message
        };

        using tcp_event_container = std::variant<tcp_event_connexion, tcp_event_disconnexion, tcp_event_message>;

        class tcp_event {
        private:
            tcp_event_container _container;

        public:
            tcp_event(tcp_event_connexion event) : _container(event) {}
            tcp_event(tcp_event_disconnexion event) : _container(event) {}
            ~tcp_event() = default;

            template<typename T>
            const T& get() const { return std::get<const T&>(_container); }

            template<typename T>
            T& get() { return std::get<T&>(_container); }

            int getType() const { return _container.index(); }
        };

        class tcp_server
        {
            public:
                tcp_server(boost::asio::io_context& io_context, int port)
                    : _io_context(io_context)
                    , _acceptor(io_context, tcp::endpoint(tcp::v4(), port))
                {
                    start_accept();
                    poll_tcp_connections();
                }

                bool poll(tcp_event& event) {
                    return _events.async_pop(event);
                }

                void send(size_t id, tcp_connection::shared_message_info_t message) {
                    auto it = _connections.async_get(id);
                    if (it)
                        it->send(message);
                    else
                        std::fprintf(stderr, "Error while sending message: connection %zu not found", id);
                }

            private:
                void start_accept()
                {
                    tcp_connection::pointer new_connection =
                        tcp_connection::create(_io_context);

                    _acceptor.async_accept(new_connection->socket(),
                            boost::bind(&tcp_server::handle_accept, this, new_connection,
                                boost::asio::placeholders::error));
                }

                void handle_accept(tcp_connection::pointer new_connection,
                        const boost::system::error_code& error)
                {
                    if (!error) {
                        new_connection->start();
                        _events.async_emplace(_connections.async_set(new_connection));
                    } else {
                        std::fprintf(stderr, "Error: %s\n", error.message().c_str());
                        return;
                    }
                    start_accept();
                }

                void poll_tcp_connections()
                {

                    _tcp_connection_polling_thread = std::unique_ptr<boost::thread>(
                        new boost::thread([this]() {
                            tcp_connection::shared_message_info_t message;
                            for (size_t i = 0; i < _connections.size(); ++i) {
                                auto connection = _connections.async_get(i);
                                if (connection->should_exit()) {
                                    _events.async_emplace(tcp_event_disconnexion(i));
                                    _connections.async_remove(i);
                                } else {
                                    while (connection->poll(message)) {
                                        _events.async_emplace(tcp_event_message(i, message));
                                    }
                                }
                            }
                            poll_tcp_connections();
                        })
                    );
                }

                boost::asio::io_context& _io_context;
                tcp::acceptor _acceptor;
                async_automated_sparse_array<tcp_connection> _connections;

                std::unique_ptr<boost::thread> _tcp_connection_polling_thread;

                async_queue<tcp_event> _events;
        };


        class udp_server
        {
            public:
                class message_info : public base_message_info<udp_buffer_t> {
                public:
                    message_info(udp_buffer_t&& buffer, size_t size)
                        : base_message_info(std::move(buffer), size)
                    {}

                    HL_AUTO_COMPLETE_CANONICAL_FORM(message_info);
                };

                using shared_message_info_t = boost::shared_ptr<message_info>;

                udp_server(boost::asio::io_context& io_context, int port)
                    : _socket(io_context, udp::endpoint(udp::v4(), port))
                {
                    start_receive();
                }

            private:
                void start_receive()
                {
                    _socket.async_read_some(
                            boost::asio::buffer(_recv_buffer),
                        [this](const boost::system::error_code& error, std::size_t bytes_transferred) {
                            if (!error) {
                                if (bytes_transferred)
                                    _recv_queue.async_emplace(
                                            shared_message_info_t(new message_info(std::move(_recv_buffer), bytes_transferred)),
                                        bytes_transferred);
                                start_receive();
                            } else {
                                std::fprintf(stderr, "Error while receiving UDP packet\n");
                            }
                        }
                    );
                }

            public:
                bool poll(shared_message_info_t& info)
                {
                    return _recv_queue.async_pop(info);
                }

                void send(shared_message_info_t message)
                {
                    size_t index = _messages.async_set(message);

                    _socket.async_send(boost::asio::buffer(message->buffer, message->size),
                        [this, index](const boost::system::error_code& error, std::size_t bytes_transferred) {
                            // Might need to check bytes_transferred
                            (void)bytes_transferred;
                            if (!error) {
                                _messages.async_remove(index);
                            } else {
                                std::fprintf(stderr, "Error while sending UDP packet\n");
                            }
                        }
                    );
                }

                udp::socket _socket;
                udp::endpoint _remote_endpoint;
                udp_buffer_t _recv_buffer;

                async_automated_sparse_array<message_info> _messages;

                async_queue<shared_message_info_t> _recv_queue;
        };

        class tcp_udp_server {
        public:
            tcp_udp_server(boost::asio::io_context& io_context, int tcp_port, int udp_port)
                : _io_context(io_context)
                , _tcp_server(io_context, tcp_port)
                , _udp_server(io_context, udp_port)
            {}

            bool tcp_poll(tcp_event& event) { return _tcp_server.poll(event); }

            bool udp_poll(udp_server::shared_message_info_t& message) { return _udp_server.poll(message); }

            void tcp_send(tcp_connection::shared_message_info_t message, size_t index) { _tcp_server.send(index, message); }

            void udp_send(udp_server::shared_message_info_t message) { _udp_server.send(message); }

            boost::asio::io_context& io_context() { return _io_context; }

        private:
            boost::asio::io_context& _io_context;
            tcp_server _tcp_server;
            udp_server _udp_server;
        };
    }
}
