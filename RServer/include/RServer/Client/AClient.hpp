#pragma once

#include "RServer/Server/Server.hpp"

namespace rtype {
namespace net {

    using shared_message_t = boost::shared_ptr<IMessage>;

    class AClient {
    public:
        /**
         * @brief Construct a new AClient object
         */
        AClient();

        /**
         * @brief Destroy the AClient object
         */
        ~AClient() = default;

        /**
         * @brief Polls the client for new messages
         * 
         * @param message The message to fill
         * @return true If a message was received
         * @return false  If no message was received
         */
        bool poll(shared_message_t& message);

        /**
         * @brief Tells whether if a service is down, the server should be restarted
         * 
         * @return true If the client is stopped
         */
        bool stopped() const;

    private:
        async_queue<shared_message_t> _queue;

    protected:
        /**
         * @brief Adds an event to the queue
         * 
         * @param message The message to add
         */
        void add_event(shared_message_t message);

        std::atomic_bool _stopped;
    };

}
}
