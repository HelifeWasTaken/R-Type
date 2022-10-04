#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <assert.h>
#include <boost/endian.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace rtype {
namespace net {

enum class message_code : uint8_t {
    DUMMY = 0,

    // special messages
    UPDATE_MSG,
    SYNC_MSG,
    CONN_INIT_REP,
    FEED_INIT,
    FEED_INIT_REP,

    // signal markers
    CONN_INIT,
    CONN_FAILED,
    CONN_OK,
    SYNC_REQ,
    SYNC_START,
    SYNC_END,
    SYNC_OK,
    REFUSED
};

enum message_type : uint8_t {
    INVALID = 0,
    SIGNAL_MARKER,
    UPDATE_MESSAGE,
    SYNC_MESSAGE,
    QUERY_MESSAGE,
    REPLY_MESSAGE,
    CONNETION_INIT_REPLY,
    FEED_INIT_REQUEST,
    FEED_INIT_REPLY
};

static inline std::unordered_map<message_code, message_type> message_type_map = {
    { message_code::DUMMY, message_type::INVALID },
    { message_code::UPDATE_MSG, message_type::UPDATE_MESSAGE },
    { message_code::SYNC_MSG, message_type::SYNC_MESSAGE },
    { message_code::CONN_INIT_REP, message_type::CONNETION_INIT_REPLY },
    { message_code::FEED_INIT, message_type::FEED_INIT_REQUEST },
    { message_code::FEED_INIT_REP, message_type::FEED_INIT_REPLY },
    { message_code::CONN_INIT, message_type::SIGNAL_MARKER },
    { message_code::CONN_FAILED, message_type::SIGNAL_MARKER },
    { message_code::CONN_OK, message_type::SIGNAL_MARKER },
    { message_code::SYNC_REQ, message_type::SIGNAL_MARKER },
    { message_code::SYNC_START, message_type::SIGNAL_MARKER },
    { message_code::SYNC_END, message_type::SIGNAL_MARKER },
    { message_code::SYNC_OK, message_type::SIGNAL_MARKER },
    { message_code::REFUSED, message_type::SIGNAL_MARKER }
};

static inline message_type get_message_type(message_code code) {
    auto a = message_type_map.find(code);
    if (a == message_type_map.end())
        return message_type::INVALID;
    return a->second;
}

static inline message_type get_message_type(uint8_t code) {
    auto a = message_type_map.find(static_cast<message_code>(code));
    if (a == message_type_map.end())
        return message_type::INVALID;
    return a->second;
}

static inline message_type get_message_type(const uint8_t *buffer) {
    return get_message_type(buffer[0]);
}

class IMessage {
    public:
        virtual void from(const std::vector<uint8_t>& buff) = 0;
        virtual const std::vector<uint8_t> serialize() const = 0;
        virtual message_code code() const = 0;
};

class SignalMarker : public IMessage {
    public:
        SignalMarker() : _code(message_code::DUMMY) {}
        SignalMarker(message_code code) : _code(code) {}

        void from(const std::vector<uint8_t>& buff) {
            _code = static_cast<message_code>(buff[0]);
        }

        static boost::shared_ptr<SignalMarker> deserialize(const uint8_t* buffer, size_t size) {
            auto msg = boost::make_shared<SignalMarker>();
            msg->from(std::vector<uint8_t>(buffer, buffer + size));
            return msg;
        }

        static boost::shared_ptr<SignalMarker> deserialize(const std::vector<uint8_t>& buff) {
            return deserialize(buff.data(), buff.size());
        }

        const std::vector<uint8_t> serialize() const {
            return std::vector<uint8_t>{ static_cast<uint8_t>(_code) };
        }

        message_code code() const {
            return _code;
        }
    private:
        message_code _code;
};

class UpdateMessage : public IMessage {
    public:
        UpdateMessage() : _sid(0), _data(), _type(0) {}

        template <typename T>
        UpdateMessage(int16_t sid, const T& data) : _sid(sid), _data(data.serialize()), _type(T::get_serializable_type()) {}

        UpdateMessage(int16_t sid, int16_t type, const std::vector<uint8_t>& data) : _sid(sid), _data(data), _type(type) {}

        void from(const std::vector<uint8_t>& buff) {
            std::memcpy(&_type, buff.data() + 1, sizeof(_type));
            boost::endian::big_to_native_inplace(_type);
            std::memcpy(&_sid, buff.data() + 1 + sizeof(_type), sizeof(_sid));
            boost::endian::big_to_native_inplace(_sid);
            _data = std::vector<uint8_t>(buff.begin() + 1 + sizeof(_type) + sizeof(_sid), buff.end());
        }

        static boost::shared_ptr<UpdateMessage> deserialize(const uint8_t* buffer, size_t size) {
            auto msg = boost::make_shared<UpdateMessage>();
            msg->from(std::vector<uint8_t>(buffer, buffer + size));
            return msg;
        }

        static boost::shared_ptr<UpdateMessage> deserialize(const std::vector<uint8_t>& buff) {
            return deserialize(buff.data(), buff.size());
        }

        const std::vector<uint8_t> serialize() const {
            uint8_t* byteReader;
            uint16_t type = boost::endian::native_to_big(_type);
            uint16_t sid = boost::endian::native_to_big(_sid);

            std::vector<uint8_t> buff;
            buff.push_back(static_cast<uint8_t>(code()));
            byteReader = reinterpret_cast<uint8_t*>(&type);
            buff.insert(buff.end(), byteReader, byteReader + sizeof(type));
            byteReader = reinterpret_cast<uint8_t*>(&sid);
            buff.insert(buff.end(), byteReader, byteReader + sizeof(sid));
            buff.insert(buff.end(), _data.begin(), _data.end());
            return buff;
        }

        message_code code() const {
            return message_code::UPDATE_MSG;
        }

        uint16_t type() const {
            return _type;
        }

        uint16_t sid() const {
            return _sid;
        }

        const std::vector<uint8_t> data() const {
            return _data;
        }

        template <typename T>
        T get() {
            return T::deserialize(_data);
        }
    private:
        uint16_t _sid;
        uint16_t _type;
        std::vector<uint8_t> _data;
};

class SyncMessage : public IMessage {
    public:
        SyncMessage() : _sid(0), _data(), _type(0) {}

        template <typename T>
        SyncMessage(int16_t sid, const T& data) : _sid(sid), _data(data.serialize()), _type(T::get_serializable_type()) {}

        SyncMessage(int16_t sid, int16_t type, const std::vector<uint8_t>& data) : _sid(sid), _data(data), _type(type) {}

        void from(const std::vector<uint8_t>& buff) {
            std::memcpy(&_type, buff.data() + 1, sizeof(_type));
            boost::endian::big_to_native_inplace(_type);
            std::memcpy(&_sid, buff.data() + 1 + sizeof(_type), sizeof(_sid));
            boost::endian::big_to_native_inplace(_sid);
            _data = std::vector<uint8_t>(buff.begin() + 1 + sizeof(_type) + sizeof(_sid), buff.end());
        }

        static boost::shared_ptr<SyncMessage> deserialize(const uint8_t* buffer, size_t size) {
            auto msg = boost::make_shared<SyncMessage>();
            msg->from(std::vector<uint8_t>(buffer, buffer + size));
            return msg;
        }

        static boost::shared_ptr<SyncMessage> deserialize(const std::vector<uint8_t>& buff) {
            return deserialize(buff.data(), buff.size());
        }

        const std::vector<uint8_t> serialize() const {
            uint8_t* byteReader;
            uint16_t type = boost::endian::native_to_big(_type);
            uint16_t sid = boost::endian::native_to_big(_sid);

            std::vector<uint8_t> buff;
            buff.push_back(static_cast<uint8_t>(code()));
            byteReader = reinterpret_cast<uint8_t*>(&type);
            buff.insert(buff.end(), byteReader, byteReader + sizeof(type));
            byteReader = reinterpret_cast<uint8_t*>(&sid);
            buff.insert(buff.end(), byteReader, byteReader + sizeof(sid));
            buff.insert(buff.end(), _data.begin(), _data.end());
            return buff;
        }

        message_code code() const {
            return message_code::SYNC_MSG;
        }

        uint16_t type() const {
            return _type;
        }

        uint16_t sid() const {
            return _sid;
        }

        const std::vector<uint8_t> data() const {
            return _data;
        }

        template <typename T>
        T get() {
            return T::deserialize(_data);
        }
    private:
        uint16_t _sid;
        uint16_t _type;
        std::vector<uint8_t> _data;
};

class QueryMessage : public IMessage {
    public:
        QueryMessage() : _code(message_code::DUMMY) {}
        QueryMessage(message_code code, const std::vector<uint8_t>& data) : _code(code), _data(data) {}

        void from(const std::vector<uint8_t>& buff) {
            _code = static_cast<message_code>(buff[0]);
            _data = std::vector<uint8_t>(buff.begin() + 1, buff.end());
        }

        static boost::shared_ptr<QueryMessage> deserialize(const uint8_t* buffer, size_t size) {
            auto msg = boost::make_shared<QueryMessage>();
            msg->from(std::vector<uint8_t>(buffer, buffer + size));
            return msg;
        }

        static boost::shared_ptr<QueryMessage> deserialize(const std::vector<uint8_t>& buff) {
            return deserialize(buff.data(), buff.size());
        }

        const std::vector<uint8_t> serialize() const {
            std::vector<uint8_t> buff;
            buff.push_back(static_cast<uint8_t>(code()));
            buff.insert(buff.end(), _data.begin(), _data.end());
            return buff;
        }

        message_code code() const {
            return _code;
        }

        const std::vector<uint8_t> data() const {
            return _data;
        }

        std::vector<uint8_t> data() {
            return _data;
        }
    private:
        message_code _code;
        std::vector<uint8_t> _data;
};

class ReplyMessage : public IMessage {
    public:
        ReplyMessage() : _code(message_code::DUMMY), _status(0) {}
        ReplyMessage(message_code code, uint8_t status, const std::vector<uint8_t>& data) : _code(code), _status(status), _data(data) {}

        void from(const std::vector<uint8_t>& buff) {
            _code = static_cast<message_code>(buff[0]);
            _status = buff[1];
            _data = std::vector<uint8_t>(buff.begin() + 2, buff.end());
        }

        static boost::shared_ptr<ReplyMessage> deserialize(const uint8_t* buffer, size_t size) {
            auto msg = boost::make_shared<ReplyMessage>();
            msg->from(std::vector<uint8_t>(buffer, buffer + size));
            return msg;
        }

        static boost::shared_ptr<ReplyMessage> deserialize(const std::vector<uint8_t>& buff) {
            return deserialize(buff.data(), buff.size());
        }

        const std::vector<uint8_t> serialize() const {
            std::vector<uint8_t> buff;
            buff.push_back(static_cast<uint8_t>(_code));
            buff.push_back(_status);
            buff.insert(buff.end(), _data.begin(), _data.end());
            return buff;
        }

        message_code code() const {
            return _code;
        }

        uint8_t status() const {
            return _status;
        }

        const std::vector<uint8_t> data() const {
            return _data;
        }
    private:
        message_code _code;
        uint8_t _status;
        std::vector<uint8_t> _data;
};

class ConnectionInitReply : public IMessage {
    public:
        ConnectionInitReply() : _playerId(0), _token(0) {}
        ConnectionInitReply(int16_t playerId, int32_t token) : _playerId(playerId), _token(token) {}

        void from(const std::vector<uint8_t>& buff) {
            std::memcpy(&_playerId, buff.data() + 1, sizeof(_playerId));
            boost::endian::big_to_native_inplace(_playerId);
            std::memcpy(&_token, buff.data() + 1 + sizeof(_playerId), sizeof(_token));
            boost::endian::big_to_native_inplace(_token);
        }

        static boost::shared_ptr<ConnectionInitReply> deserialize(const uint8_t* buffer, size_t size) {
            auto msg = boost::make_shared<ConnectionInitReply>();
            msg->from(std::vector<uint8_t>(buffer, buffer + size));
            return msg;
        }

        static boost::shared_ptr<ConnectionInitReply> deserialize(const std::vector<uint8_t>& buff) {
            return deserialize(buff.data(), buff.size());
        }

        const std::vector<uint8_t> serialize() const {
            uint8_t* byteReader;
            uint16_t playerId = boost::endian::native_to_big(_playerId);
            uint32_t token = boost::endian::native_to_big(_token);

            std::vector<uint8_t> buff;
            buff.push_back(static_cast<uint8_t>(code()));
            byteReader = reinterpret_cast<uint8_t*>(&playerId);
            buff.insert(buff.end(), byteReader, byteReader + sizeof(playerId));
            byteReader = reinterpret_cast<uint8_t*>(&token);
            buff.insert(buff.end(), byteReader, byteReader + sizeof(token));
            return buff;
        }

        message_code code() const {
            return message_code::CONN_INIT_REP;
        }

        uint16_t playerId() const {
            return _playerId;
        }

        uint32_t token() const {
            return _token;
        }
    private:
        uint16_t _playerId;
        uint32_t _token;
};

class FeedInitRequest : public IMessage {
    public:
        FeedInitRequest() : _playerId(0), _token(0) {}
        FeedInitRequest(int16_t playerId, int32_t token) : _playerId(playerId), _token(token) {}

        void from(const std::vector<uint8_t>& buff) {
            std::memcpy(&_playerId, buff.data() + 1, sizeof(_playerId));
            boost::endian::big_to_native_inplace(_playerId);
            std::memcpy(&_token, buff.data() + 1 + sizeof(_playerId), sizeof(_token));
            boost::endian::big_to_native_inplace(_token);
        }

        static boost::shared_ptr<FeedInitRequest> deserialize(const uint8_t* buffer, size_t size) {
            auto msg = boost::make_shared<FeedInitRequest>();
            msg->from(std::vector<uint8_t>(buffer, buffer + size));
            return msg;
        }

        static boost::shared_ptr<FeedInitRequest> deserialize(const std::vector<uint8_t>& buff) {
            return deserialize(buff.data(), buff.size());
        }

        const std::vector<uint8_t> serialize() const {
            uint8_t* byteReader;
            int16_t playerId = boost::endian::native_to_big(_playerId);
            int32_t token = boost::endian::native_to_big(_token);

            std::vector<uint8_t> buff;
            buff.push_back(static_cast<uint8_t>(code()));
            byteReader = reinterpret_cast<uint8_t*>(&playerId);
            buff.insert(buff.end(), byteReader, byteReader + sizeof(playerId));
            byteReader = reinterpret_cast<uint8_t*>(&token);
            buff.insert(buff.end(), byteReader, byteReader + sizeof(token));
            return buff;
        }

        message_code code() const {
            return message_code::FEED_INIT;
        }

        uint16_t playerId() const {
            return _playerId;
        }

        uint32_t token() const {
            return _token;
        }
    private:
        uint16_t _playerId;
        uint32_t _token;
};

class FeedInitReply : public IMessage {
    public:
        FeedInitReply() : _token(0) {}
        FeedInitReply(int32_t token) : _token(token) {}

        void from(const std::vector<uint8_t>& buff) {
            std::memcpy(&_token, buff.data() + 1, sizeof(_token));
            boost::endian::big_to_native_inplace(_token);
        }

        static boost::shared_ptr<FeedInitReply> deserialize(const uint8_t* buffer, size_t size) {
            auto msg = boost::make_shared<FeedInitReply>();
            msg->from(std::vector<uint8_t>(buffer, buffer + size));
            return msg;
        }

        static boost::shared_ptr<FeedInitReply> deserialize(const std::vector<uint8_t>& buff) {
            return deserialize(buff.data(), buff.size());
        }

        const std::vector<uint8_t> serialize() const {
            uint8_t* byteReader;
            std::vector<uint8_t> buff;
            uint32_t token = boost::endian::native_to_big(_token);

            buff.push_back(static_cast<uint8_t>(code()));
            byteReader = reinterpret_cast<uint8_t*>(&token);
            buff.insert(buff.end(), byteReader, byteReader + sizeof(token));
            return buff;
        }

        message_code code() const {
            return message_code::FEED_INIT_REP;
        }

        int32_t token() const {
            return _token;
        }
    private:
        uint32_t _token;
};

static inline boost::shared_ptr<IMessage> parse_message(const uint8_t* buffer, size_t size) {
    assert(size > 0);
    message_code code = static_cast<message_code>(buffer[0]);
    message_type type = get_message_type(buffer[0]);
    switch (type) {
        case message_type::UPDATE_MESSAGE:
            return UpdateMessage::deserialize(buffer, size);
        case message_type::SYNC_MESSAGE:
            return SyncMessage::deserialize(buffer, size);
        case message_type::CONNETION_INIT_REPLY:
            return ConnectionInitReply::deserialize(buffer, size);
        case message_type::FEED_INIT_REQUEST:
            return FeedInitRequest::deserialize(buffer, size);
        case message_type::FEED_INIT_REPLY:
            return FeedInitReply::deserialize(buffer, size);
        case message_type::SIGNAL_MARKER:
            return SignalMarker::deserialize(buffer, size);
        default:
            return nullptr;
    }
}

static inline boost::shared_ptr<IMessage> parse_message(const std::vector<uint8_t>& buff) {
    return parse_message(buff.data(), buff.size());
}

template <typename T>
static inline boost::shared_ptr<T> parse_message(const uint8_t* buffer, size_t size) {
    auto message = parse_message(buffer, size);
    if (dynamic_cast<T*>(message.get()) != nullptr) {
        return boost::static_pointer_cast<T>(message);
    }
    return nullptr;
}

template <typename T>
static inline boost::shared_ptr<T> parse_message(const std::vector<uint8_t>& buff) {
    return parse_message<T>(buff.data(), buff.size());
}

}
}
