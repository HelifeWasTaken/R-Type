#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {
    TextMessage::TextMessage(const std::string& text)
        : Message(message_code::TEXT_MESSAGE)
        , _text(text)
    {}

    void TextMessage::from(const uint8_t *data, const size_t size) {
        Serializer s(data, size);
        s >> _message_code;
        _text = std::string(s.data.begin(), s.data.end());
    }

    std::vector<uint8_t> TextMessage::serialize() const {
        Serializer s;
        s << _message_code;
        s.add_bytes(_text);
        return s.data;
    }

    const std::string& TextMessage::text() const { return _text; }

}
}