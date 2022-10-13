#pragma once

#include <assert.h>
#include <boost/endian.hpp>
#include <boost/make_shared.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <memory>
#include <unordered_map>
#include <vector>
#include <cstdlib>

#define RTYPE_PLAYER_COUNT 4

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
        Serializable() = default;

        uint8_t type() const { return _type; }

    protected:
        uint8_t _type = 0xFF;
    };

    enum class message_code : uint8_t {
        DUMMY = 0,

        // signal markers
        CONN_INIT,
        CREATE_ROOM,

        // special messages
        CONN_INIT_REP,
        FEED_INIT,
        FEED_INIT_REP,

        TEXT_MESSAGE,
        REQUEST_CONNECT_ROOM,
        CREATE_ROOM_REPLY,
        CONNECT_ROOM_REQ_REP,
        ROOM_CLIENT_CONNECT,
        ROOM_CLIENT_DISCONNECT,

        // Sync messages
        SYNC_VECTOR2_POSITION,

        // Update messages
        UPDATE_VECTOR2_MOVEMENT,
    };

    // All classes that inherit from IMessage
    // Is only used to cast the message to the right type
    enum message_type : uint8_t {
        INVALID = 0,

        // Signal markers
        SIGNAL_MARKER,
        CONN_INIT = SIGNAL_MARKER,
        CREATE_ROOM = SIGNAL_MARKER,

        CONNECTION_INIT_REPLY,
        CONN_INIT_REP = CONNECTION_INIT_REPLY,

        FEED_INIT_REQUEST,
        FEED_INIT = FEED_INIT_REQUEST,

        FEED_INIT_REPLY,
        FEED_INIT_REP = FEED_INIT_REPLY,

        TEXT_MESSAGE,
        REQUEST_CONNECT_ROOM,
        CREATE_ROOM_REPLY,
        CONNECT_ROOM_REQ_REP,
        ROOM_CLIENT_CONNECT,
        ROOM_CLIENT_DISCONNECT,

        // Sync messages
        SYNC_MESSAGE,
        SYNC_VECTOR2_POSITION = SYNC_MESSAGE,

        // Special messages
        UPDATE_MESSAGE,
        UPDATE_VECTOR2_MOVEMENT = UPDATE_MESSAGE,
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
        try {
            return boost::dynamic_pointer_cast<T>(parse_message(buffer, size));
        } catch (...) {
            return nullptr;
        }
    }

    template<typename T>
    boost::shared_ptr<T> parse_message(const std::vector<uint8_t>& buff) {
        try {
            return boost::dynamic_pointer_cast<T>(parse_message(buff));
        } catch (...) {
            return nullptr;
        }
    }

    template<typename T>
    boost::shared_ptr<T> parse_message(boost::shared_ptr<IMessage> msg) {
        return boost::dynamic_pointer_cast<T>(msg);
    }

    template<typename T>
    T *parse_message(IMessage *msg) {
        return dynamic_cast<T*>(msg);
    }

    template<typename T>
    const T* parse_message(const IMessage *msg) {
        return dynamic_cast<const T*>(msg);
    }

    template<typename T>
    T *parse_message(IMessage &msg) {
        return parse_message<T>(&msg);
    }

    template<typename T>
    const T* parse_message(const IMessage &msg) {
        return parse_message<T>(&msg);
    }

    message_type message_code_to_type(const message_code& code);
    message_code message_type_to_code(const message_type& type);

    namespace token {
        #ifndef RTYPE_TOKEN_SIZE
            #define RTYPE_TOKEN_SIZE 6 
        #endif
        std::string generate_token(void);
    }

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
        UpdateMessage(const uint8_t& sid, const Serializable& data, const message_code& code);

        void from(const uint8_t *data, const size_t size) override;
        std::vector<uint8_t> serialize() const override;

        uint8_t sid() const;
        const std::vector<uint8_t>& data() const;

    private:
        uint8_t _sid = 0;
        std::vector<uint8_t> _data;
    };

    class SyncMessage : public Message {
    public:
        SyncMessage() = default;
        SyncMessage(uint8_t sid, const Serializable& serializable, const message_code& code);

        void from(const uint8_t *data, const size_t size) override;
        std::vector<uint8_t> serialize() const override;

        uint8_t sid() const;
        const std::vector<uint8_t>& data() const;

    private:
        std::vector<uint8_t> _data;
        uint8_t _sid = 0;
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

    class CreateRoomReply : public Message {
    public:
        CreateRoomReply() = default;
        CreateRoomReply(const std::string& token);

        void from(const uint8_t *data, const size_t size) override;
        std::vector<uint8_t> serialize() const override;

        const std::string& token() const;

    private:
        std::string _token = "";
    };

    class UserConnectRoom : public Message {
    public:
        UserConnectRoom() = default;
        UserConnectRoom(uint8_t playerID);

        void from(const uint8_t *data, const size_t size) override;
        std::vector<uint8_t> serialize() const override;

        uint8_t playerID() const;
    private:
        uint8_t _playerID = 0;
    };

    class UserDisconnectFromRoom : public Message {
    public:
        UserDisconnectFromRoom() = default;
        UserDisconnectFromRoom(size_t dc_user, size_t new_host);

        void from(const uint8_t *data, const size_t size) override;
        std::vector<uint8_t> serialize() const override;

        const size_t get_disconnected_user_id() const;
        const size_t get_new_host_id() const;

    private:
        size_t _dc_user_id = 0;
        size_t _new_host_id = 0;
    };

}
}