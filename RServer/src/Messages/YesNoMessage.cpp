#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    YesNoMarker::YesNoMarker(const message_code& code, const bool& yesno)
        : Message(code)
        , _yes(yesno)
    {
    }

    BufferSizeType YesNoMarker::size() const
    {
        return sizeof(_message_code) + sizeof(_yes);
    }

    void YesNoMarker::from(const Byte *data, BufferSizeType size)
    {
        Serializer s(data, size);
        s >> _message_code >> _yes;
    }

    std::vector<Byte> YesNoMarker::serialize() const
    {
        Serializer s;

        s << _message_code << _yes;
        return s.data;
    }

    Bool YesNoMarker::yes() const
    {
        return _yes;
    }
}
}
