#pragma once

#include <assert.h>
#include <boost/endian.hpp>
#include <boost/make_shared.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace rtype {
namespace net {

    struct Serializer {
        std::vector<uint8_t> data;

        Serializer(const uint8_t *bytes, size_t size)
            : data(bytes, bytes + size)
        {}

        Serializer() = default;

        template<typename T>
        Serializer& operator<<(const T& value) {
            const T big_value = boost::endian::native_to_big(value);
            const uint8_t* byteReader = reinterpret_cast<const uint8_t*>(&big_value);
            data.insert(data.end(), byteReader, byteReader + sizeof(T));
            return *this;
        }

        template<typename T>
        Serializer& operator>>(T& value) {
            if (data.size() < sizeof(T))
                throw std::runtime_error("Cannot extract data");
            std::memcpy(&value, data.data(), sizeof(T));
            value = boost::endian::native_to_big(value);
            data.erase(data.begin(), data.begin() + sizeof(T));
            return *this;
        }

        Serializer& add_bytes(const uint8_t *bytes, size_t size) {
            data.insert(data.end(), bytes, bytes + size);
            return *this;
        }

        Serializer& add_bytes(const std::vector<uint8_t>& bytes) {
            return add_bytes(bytes.data(), bytes.size());
        }

        Serializer& add_bytes(const std::string& str) {
            return add_bytes(reinterpret_cast<const uint8_t*>(str.c_str()), str.size());
        }
    };

    class Serializable {
    public:
        virtual std::vector<uint8_t> serialize() const = 0;
        virtual void from(const uint8_t *data, const size_t size) = 0;

        template<typename T>
        static T deserialize(const uint8_t *data, const size_t size) {
            T t;
            t.from(data, size);
            return t;
        }

        template<typename T>
        static T deserialize(const std::vector<uint8_t>& data) {
            return from<T>(data.data(), data.size());
        }

        Serializable(int8_t type) : _type(type) {}

        int8_t type() const { return _type; }

    private:
        int8_t _type;
    };

    enum class message_code : uint8_t {
        DUMMY = 0,

        // signal markers
        CONN_INIT,

        // special messages
        CONN_INIT_REP,
        FEED_INIT,
        FEED_INIT_REP,
        SYNC_MESSAGE,
        UPDATE_MESSAGE,
        TEXT_MESSAGE,
        REQUEST_CONNECT_ROOM
    };

    // All classes that inherit from IMessage
    // Is only used to cast the message to the right type
    enum message_type : uint8_t {
        INVALID = 0,
        SIGNAL_MARKER,
        UPDATE_MESSAGE,
        SYNC_MESSAGE,
        CONNECTION_INIT_REPLY,
        FEED_INIT_REQUEST,
        FEED_INIT_REPLY,
        TEXT_MESSAGE,
        REQUEST_CONNECT_ROOM
    };

    class IMessage {
    public:
        virtual void from(const uint8_t *data, const size_t size) = 0;
        // virtual void from(const std::vector<uint8_t>& buff) = 0;
        virtual std::vector<uint8_t> serialize() const = 0;
        virtual message_type type() const = 0;
        virtual message_code code() const = 0;
    };

    // ParseMessage file content
    boost::shared_ptr<IMessage> parse_message(const uint8_t* buffer, size_t size);
    boost::shared_ptr<IMessage> parse_message(const std::vector<uint8_t>& buff);

    template<typename T>
    boost::shared_ptr<T> parse_message(const uint8_t* buffer, size_t size) {
        return boost::dynamic_pointer_cast<T>(parse_message(buffer, size));
    }

    template<typename T>
    boost::shared_ptr<T> parse_message(const std::vector<uint8_t>& buff) {
        return boost::dynamic_pointer_cast<T>(parse_message(buff));
    }

    template<typename T>
    boost::shared_ptr<T> parse_message(boost::shared_ptr<IMessage> msg) {
        return boost::dynamic_pointer_cast<T>(msg);
    }

    message_type message_code_to_type(const message_code& code);
    message_code message_type_to_code(const message_type& type);

    // Start of the different messages

    class Message : public IMessage {
    public:
        Message(message_code code) :
            _message_code(code) {}
        Message() = default;

        ~Message() = default;

        template<typename T>
        static boost::shared_ptr<T> deserialize(const uint8_t *data, const size_t size)
        {
            boost::shared_ptr<T> msg = boost::make_shared<T>();
            msg->from(data, size);
            return msg;
        }

        template<typename T>
        static boost::shared_ptr<T> deserialize(const std::vector<uint8_t>& buff)
        { return deserialize<T>(buff.data(), buff.size()); }

        message_type type() const override final
        { return message_code_to_type(_message_code); }

        message_code code() const override final
        { return _message_code; }

    protected:
        message_code _message_code = message_code::DUMMY;
    };

    class SignalMarker : public Message {
    public:
        SignalMarker() = default;
        SignalMarker(const message_code& code);

        void from(const uint8_t *data, const size_t size) override;
        std::vector<uint8_t> serialize() const override;
    };

    class UpdateMessage : public Message {
    public:
        UpdateMessage() = default;
        UpdateMessage(const int16_t& sid, const Serializable& data);

        void from(const uint8_t *data, const size_t size) override;
        std::vector<uint8_t> serialize() const override;

        uint16_t sid() const;
        const std::vector<uint8_t>& data() const;

    private:
        uint16_t _sid = 0;
        std::vector<uint8_t> _data;
    };

    class SyncMessage : public Message {
    public:
        SyncMessage() = default;
        SyncMessage(int16_t sid, const Serializable& serializable);

        void from(const uint8_t *data, const size_t size) override;
        std::vector<uint8_t> serialize() const override;

        uint16_t sid() const;
        const std::vector<uint8_t>& data() const;

    private:
        std::vector<uint8_t> _data;
        uint16_t _sid = 0;
    };

    class ConnectionInitReply : public Message {
    public:
        ConnectionInitReply() = default;
        ConnectionInitReply(int16_t playerId, int32_t token);

        void from(const uint8_t *data, const size_t size) override;
        std::vector<uint8_t> serialize() const override;

        uint16_t playerId() const;
        uint32_t token() const;

    private:
        uint16_t _playerId = 0;
        uint32_t _token = 0;
    };

    class FeedInitRequest : public Message {
    public:
        FeedInitRequest() = default;
        FeedInitRequest(int16_t playerId, int32_t token);

        void from(const uint8_t *data, const size_t size) override;
        std::vector<uint8_t> serialize() const override;

        uint16_t playerId() const;
        uint32_t token() const;

    private:
        uint16_t _playerId = 0;
        uint32_t _token = 0;
    };

    class FeedInitReply : public Message {
    public:
        FeedInitReply() = default;
        FeedInitReply(int32_t token);

        void from(const uint8_t *data, const size_t size) override;
        std::vector<uint8_t> serialize() const override;

        uint32_t token() const;

    private:
        uint32_t _token = 0;
    };

    class TextMessage : public Message {
    public:
        TextMessage() = default;
        TextMessage(const std::string& text);

        void from(const uint8_t *data, const size_t size) override;
        std::vector<uint8_t> serialize() const override;

        const std::string& text() const;
    private:
        std::string _text;
    };

    class RequestConnectRoom : public Message {
    public:
        RequestConnectRoom() = default;
        RequestConnectRoom(const std::string& roomID);

        void from(const uint8_t *data, const size_t size) override;
        std::vector<uint8_t> serialize() const override;

        const std::string& roomID() const;
    private:
        std::string _roomID;
    };

    #define RTYPE_INVALID_PLAYER_ID 0xFF
    /**
     * @brief If _playerID is 0xFF then the room is full
     *        or room does not exists
     */
    class RequestConnectRoomReply : public Message {
    public:
        RequestConnectRoomReply() = default;
        RequestConnectRoomReply(uint8_t playerID);

        void from(const uint8_t *data, const size_t size) override;
        std::vector<uint8_t> serialize() const override;

        uint8_t playerID() const;

    private:
        uint8_t _playerID = RTYPE_INVALID_PLAYER_ID;
    };

}
}
