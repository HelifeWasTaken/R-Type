#include "RServer/Client/AClient.hpp"

namespace rtype {
namespace net {

    AClient::AClient()
    {
        _stopped = false;
    }

    bool AClient::stopped() const
    {
        return _stopped;
    }

    //Abstract Client class implémentation
    bool AClient::poll(shared_message_t& message)
    {
        return _queue.async_pop(message);
    }

    void AClient::add_event(shared_message_t message)
    {
        return _queue.async_push(message);
    }

}
}