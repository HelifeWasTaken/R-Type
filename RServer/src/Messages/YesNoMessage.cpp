#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    YesNoMarker::YesNoMarker(const message_code& code, const bool& yesno)
        : Message(code)
        , _yes(yesno)
    {
    }

    void YesNoMarker::from(const uint8_t *data, size_t size)
    {
        Serializer s(data, size);
        int8_t yes;

        s >> _message_code >> yes;
        _yes = yes;
    }

    std::vector<uint8_t> YesNoMarker::serialize() const
    {
        Serializer s;
        int8_t yes = _yes;

        s << _message_code << yes;
        return s.data;
    }

}
}
