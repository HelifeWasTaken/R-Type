#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    YesNoMarker::YesNoMarker(const message_code& code, const bool& yesno)
        : Message(code)
        , _yes(yesno)
    {
    }

    void YesNoMarker::from(const Byte *data, BufferSizeType size)
    {
        Serializer s(data, size);
        int8_t yes;

        s >> _message_code >> yes;
        _yes = yes;
    }

    std::vector<Byte> YesNoMarker::serialize() const
    {
        Serializer s;
        Byte yes = _yes;

        s << _message_code << yes;
        return s.data;
    }

}
}
