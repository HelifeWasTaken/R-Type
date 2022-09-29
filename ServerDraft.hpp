#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <queue>
#include <variant>

// TODO: Use proper logging library vs std::fprintf stderr

namespace rtype {
    namespace net {

        template<typename T>
        class safe_queue : public std::queue<T> {
        public:
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
            safe_queue<size_t> _unused_indexes;
            std::vector<boost::shared_ptr<T>> _array;

        public:
            async_automated_sparse_array() {}

            boost::shared_ptr<T> async_get(size_t index) {
                std::lock_guard<std::mutex> lock(_mut);
                if (index >= this->size())
                    return nullptr;
                return this->at(index);
            }

            size_t async_set(boost::shared_ptr<T> value) {
                std::lock_guard<std::mutex> lock(_mut);
                size_t index;

                if (_unused_indexes.async_pop(index)) {
                    this->_array.at(index) = value;
                    return index;
                }
                this->_array.push_back(value);
                return this->_array.size() - 1;
            }

            void async_remove(size_t index) {
                std::lock_guard<std::mutex> lock(_mut);
                if (index >= this->size())
                    return;
                this->at(index) = nullptr;
                _unused_indexes.async_push(index);
            }
        };

#ifndef RTYPE_tcp__BUFFER_SIZE
#define RTYPE_tcp__BUFFER_SIZE 1024
#endif

#ifndef RTYPE_UDP_BUFFER_SIZE
#define RTYPE_UDP_BUFFER_SIZE 1024
#endif

        using tcp = boost::asio::ip::tcp;
        using udp = boost::asio::ip::tcp;

        using tcp_buffer_t = boost::array<char, RTYPE_tcp__BUFFER_SIZE>;
        using udp_buffer_t = boost::array<char, RTYPE_UDP_BUFFER_SIZE>;

        class tcp_connection
            : public boost::enable_shared_from_this<tcp_connection>
        {
            public:
                using pointer = boost::shared_ptr<tcp_connection>;

                static pointer create(boost::asio::io_context& io_context)
                {
                    return pointer(new tcp_connection(io_context));
                }

                tcp::socket& socket()
                {
                    return _socket;
                }

                void start()
                {
                }

            private:
                tcp_connection(boost::asio::io_context& io_context)
                    : _socket(io_context)
                {
                }

                void handle_write()
                {
                }

                tcp::socket _socket;
        };

        class tcp_event_connexion {
        private:
            size_t _id = -1;

        public:
            tcp_event_connexion(size_t id) : _id(id) {}
            size_t getId() const { return _id; }
        };

        class tcp_event_disconnexion {
        private:
            size_t _id = -1;
        public:
            tcp_event_disconnexion(size_t id) : _id(id) {}
            size_t getId() const { return _id; }
        };

        enum class tcp_event_type {
            Connexion,
            Disconnexion
        };

        using tcp_event_container = std::variant<tcp_event_connexion, tcp_event_disconnexion>;

        class tcp_event {
        private:
            tcp_event_container _container;

        public:
            tcp_event(tcp_event_connexion event) : _event(event) {}
            tcp_event(tcp_event_disconnexion event) : _event(event) {}

            template<typename T>
            const T& get() const { return std::get<const T&>(_event); }

            template<typename T>
            T& get() { return std::get<T&>(_event); }

            int getType() const { return _event.index(); }
        };

        class tcp_server
        {
            public:
                tcp_server(boost::asio::io_context& io_context, int port)
                    : _io_context(io_context)
                    , _acceptor(io_context, tcp::endpoint(tcp::v4(), port))
                {
                    start_accept();
                }

                bool poll(tcp_event& event) {
                    return _events.async_pop(event);
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
                    } else if (boost::asio::error::eof == error || boost::asio::error::connection_reset == error) {
                        size_t id = _connections.async_set(new_connection);
                        _events.async_emplace(tcp_event_disconnexion(id));
                        std::fprintf(stderr, "Connection %zu closed\n", id);
                    } else {
                        std::fprintf(stderr, "Error: %s\n", error.message().c_str());
                        return;
                    }
                    start_accept();
                }

                boost::asio::io_context& _io_context;
                tcp::acceptor _acceptor;
                async_automated_sparse_array<tcp_connection> _connections;

                safe_queue<tcp_event> _events;
        };


        class udp_server
        {
            public:
                struct MessageInfo {
                    udp_buffer_t buffer;
                    size_t size = 0;

                    MessageInfo(udp_buffer_t&& buffer, size_t size)
                        : buffer(std::move(buffer))
                        , size(size)
                    {}

                    MessageInfo(const MessageInfo& other) = default;
                    MessageInfo(MessageInfo&& other) = default;

                    MessageInfo() = default;
                };

                using shared_message_t = boost::shared_ptr<MessageInfo>;

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
                                _recv_queue.async_emplace(std::move(_recv_buffer), bytes_transferred);
                                start_receive();
                            } else {
                                std::fprintf(stderr, "Error while receiving UDP packet\n");
                            }
                        }
                    );
                }

            public:
                bool poll(MessageInfo& info)
                {
                    return _recv_queue.async_pop(info);
                }

                void send(shared_message_t message, std::atomic_bool& finished)
                {
                    finished = false;
                    _socket.async_send(boost::asio::buffer(message->buffer, message->size),
                        [&finished](const boost::system::error_code& error, std::size_t bytes_transferred) {
                            finished = true;
                            if (error) {
                                std::fprintf(stderr, "Error while sending message\n");
                            }
                        }
                    );
                }

                udp::socket _socket;
                udp::endpoint _remote_endpoint;
                udp_buffer_t _recv_buffer;

                safe_queue<MessageInfo> _recv_queue;
        };
    }
}
