# Messaging

## Definitions

- an int16 is defined as an integer number, encoded in **2 bytes**, in **big endian**.
- an int32 is defined as an integer number, encoded in **4 bytes**, in **big endian**.
- an int64 is defined as an integer number, encoded in **8 bytes**, in **big endian**.
- a string, is defined as a sequence of bytes. it should be **ASCII** compliant.
- `CODE` always refer to a **single byte** code.
- `STATUS` always refer to a **single byte** status.
- `SID` is a shared identifier that refers to **the same element for both the server and every client**, it is encoded as an **int16**.

## Channels

Network communication is separated into two main channels: `main` and `feed` channels.

### `main` channel

`main` is in TCP. It is used to transmit data in a safe way and transmit resynchronization messages and important messages.

### `feed` channel

`feed` is in UDP. It is used to transmit real-time state changes faster, with possible loss of precision. Using it in pair with the `main` channel synchronization messages can compensate the loss of precision.

Any message sent on the `feed` channel should be preceded by a sequence number, to ensure that older messages are not received after newer ones.

```
0000000AAAAAAAABBBBBBBBBDDDDDDDDDDDDDDDDDD
 |       |        |             |
MAGIC   SEQ    PLAYERID       CONTENT

MAGIC is a magic number, as an int64, used to ensure that the message is valid. its value is 0xff1cec0ffeedefec.
SEQ is the sequence number, encoded as an int64, it is incremented by one for each message sent.
PLAYERID is the sender player ID, encoded as a int16, used to identify the host that sent the message.
CONTENT is the message content, it should never be larger than 500 bytes for any message, otherwise behaviour is undefined.
```

If `TO` is `65535` (`0xffff`), the message is adressed to all clients. If `TO` is non-zero, the message is adressed to the client with the corresponding ID. If `TO` is `0`, the message is adressed to the server.

Messages in the `feed` channel that does not match this format should be ignored.

## Message types

Please note that some messages types MUST be available on both channels, while others MUST be available only on one channel.

Here is a very basic and partial message interface implementation:
```cpp
class IMessage {
    public:
        virtual void from(const std::vector<char>& buff) = 0;
        virtual std::vector<char> bytes() = 0; // note: the first byte is always the code
        virtual char code() = 0;
};
```

### Signal markers

They can be sent both in `main` and `feed` channels.

Signal markers are single byte messages.

```
AAAA
 |
CODE

CODE is the signal marker message code.
```

Sample implementation:
```cpp
class SignalMarker : public IMessage {
    public:
        SignalMarker() : _code(0) {}
        SignalMarker(char code) : _code(code) {}

        void from(const std::vector<char>& buff) {
            _code = buff[0];
        }

        std::vector<char> bytes() {
            return std::vector<char>{_code};
        }

        char code() {
            return code;
        }
    private:
        char _code;
};
```

### Yes No marker

Yes No markers are two bytes messages with a code and a boolean anwser.

```
AAAABBBBBB
 |    |
CODE BOOL

CODE is the message code
BOOL is the boolean anwser
```


### Update message (specifically `UPDATE_MSG`)

The channel used is the `feed` channel.

```
AAAABBBBBCCCCCCCCDDDDDDDDDDDDDDDD
 |    |      |       |
CODE TYPE   SID     DATA

CODE is the message code, it has the value of `UPDATE_MSG` (refer to the relevant code table).
TYPE is the type code of the data, it is encoded as an int8.
SID is the shared identifier, it should refer to the same element for the server and for every client.
DATA is the serialized data which is relevant for this SID. its size in bytes is unspecified and can depend on the relevant TYPE.
```

Sample implementation:
```cpp

template <typename T>
class UpdateMessage : public IMessage {
    public:
        UpdateMessage() : _sid(0) {}
        UpdateMessage(int16_t sid, const T& data) : _sid(sid), _data(data.serialize()) {}
        UpdateMessage(int16_t sid, const std::vector<char>& data) : _sid(sid), _data(data) {}

        void from(const std::vector<char>& buff) {
            _sid = (buff[1] << 8) | buff[2];
            _data = std::vector<char>(buff.begin() + 3, buff.end());
        }

        std::vector<char> bytes() {
            std::vector<char> buff;
            buff.push_back(UPDATE_MSG);
            buff.push_back(type());
            buff.push_back(_sid >> 8);
            buff.push_back(_sid & 0xff);
            buff.insert(buff.end(), _data.begin(), _data.end());
            return buff;
        }

        char code() {
            return UPDATE_MSG;
        }

        int8_t type() {
            return T::type();
        }

        int16_t sid() {
            return _sid;
        }

        std::vector<char> data() {
            return _data;
        }

        T get() {
            return T::deserialize(_data);
        }
    private:
        int16_t _sid;
        std::vector<char> _data;
};
```

### Synchronization message (specifically `SYNC_MSG`)

The channel used is the `main` channel.

```
AAAABBBBBCCCCCCCCDDDDDDDDDDDDDDDD
 |    |      |       |
CODE TYPE   SID     DATA

CODE is the message code, it has the value of `SYNC_MSG` (refer to the relevant code table).
TYPE is the type code of the data, it is encoded as an int8.
SID is the shared identifier, it should refer to the same element for the server and for every client.
DATA is the serialized data which is relevant for this SID. its size in bytes is unspecified and can depend on the relevant TYPE.
```

Sample implementation:
```cpp

template <typename T>
class SyncMessage : public IMessage {
    public:
        SyncMessage() : _sid(0) {}
        SyncMessage(int16_t sid, const T& data) : _sid(sid), _data(data.serialize()) {}
        SyncMessage(int16_t sid, const std::vector<char>& data) : _sid(sid), _data(data) {}

        void from(const std::vector<char>& buff) {
            _sid = (buff[1] << 8) | buff[2];
            _data = std::vector<char>(buff.begin() + 3, buff.end());
        }

        std::vector<char> bytes() {
            std::vector<char> buff;
            buff.push_back(SYNC_MSG);
            buff.push_back(type());
            buff.push_back(_sid >> 8);
            buff.push_back(_sid & 0xff);
            buff.insert(buff.end(), _data.begin(), _data.end());
            return buff;
        }

        char code() {
            return SYNC_MSG;
        }

        int8_t type() {
            return T::type();
        }

        int16_t sid() {
            return _sid;
        }

        std::vector<char> data() {
            return _data;
        }

        T get() {
            return T::deserialize(_data);
        }
    private:
        int16_t _sid;
        std::vector<char> _data;
};
```

### Connection init reply (specifically `CONN_INIT_REP`)

Replies a player ID and a token.

```
AAAABBBBBBBBBBCCCCCCCCCCCCC
 |    |             |
CODE PLAYERID     TOKEN

CODE is the message code, it has the value of `CONN_INIT_REP` (refer to the relevant code table).
PLAYERID is the player ID, it is encoded as an int16.
TOKEN is the token, it is encoded as an int32.
```


Sample implementation:
```cpp

class ConnectionInitReply : public IMessage {
    public:
        ConnectionInitReply() : _playerId(0), _token(0) {}
        ConnectionInitReply(int16_t playerId, int32_t token) : _playerId(playerId), _token(token) {}

        void from(const std::vector<char>& buff) {
            _playerId = (buff[0] << 8) | buff[1];
            _token = (buff[2] << 24) | (buff[3] << 16) | (buff[4] << 8) | buff[5];
        }

        std::vector<char> bytes() {
            std::vector<char> buff;
            buff.push_back(CONN_INIT_REP);
            buff.push_back(_playerId >> 8);
            buff.push_back(_playerId & 0xff);
            buff.push_back(_token >> 24);
            buff.push_back((_token >> 16) & 0xff);
            buff.push_back((_token >> 8) & 0xff);
            buff.push_back(_token & 0xff);
            return buff;
        }

        char code() {
            return CONN_INIT_REP;
        }

        int16_t playerId() {
            return _playerId;
        }

        int32_t token() {
            return _token;
        }
    private:
        int16_t _playerId;
        int32_t _token;
};
```

### Feed init request (specifically `FEED_INIT`)

Sent by the client to the server to request the `feed` channel init.

```
AAAACCCCCCCCCCCCC
 |        |
CODE     TOKEN

CODE is the message code, it has the value of `FEED_INIT` (refer to the relevant code table).
TOKEN is the token, it is encoded as an int32.
```

Sample implementation:
```cpp

class FeedInitRequest : public IMessage {
    public:
        FeedInitRequest() : _playerId(0), _token(0) {}
        FeedInitRequest(int16_t playerId, int32_t token) : _playerId(playerId), _token(token) {}

        void from(const std::vector<char>& buff) {
            _playerId = (buff[0] << 8) | buff[1];
            _token = (buff[2] << 24) | (buff[3] << 16) | (buff[4] << 8) | buff[5];
        }

        std::vector<char> bytes() {
            std::vector<char> buff;
            buff.push_back(FEED_INIT);
            buff.push_back(_playerId >> 8);
            buff.push_back(_playerId & 0xff);
            buff.push_back(_token >> 24);
            buff.push_back((_token >> 16) & 0xff);
            buff.push_back((_token >> 8) & 0xff);
            buff.push_back(_token & 0xff);
            return buff;
        }

        char code() {
            return FEED_INIT;
        }

        int16_t playerId() {
            return _playerId;
        }

        int32_t token() {
            return _token;
        }
    private:
        int16_t _playerId;
        int32_t _token;
};
```

### Feed init reply (specifically `FEED_INIT_REP`)

Sent by the server to the client to reply the `feed` channel init.

```
AAAACCCCCCCCCCCCC
 |       |
CODE   TOKEN

CODE is the message code, it has the value of `FEED_INIT_REP` (refer to the relevant code table).
TOKEN is the token, it is encoded as an int32.
```

Sample implementation:
```cpp

class FeedInitReply : public IMessage {
    public:
        FeedInitReply() : _token(0) {}
        FeedInitReply(int32_t token) : _token(token) {}

        void from(const std::vector<char>& buff) {
            _token = (buff[0] << 24) | (buff[1] << 16) | (buff[2] << 8) | buff[3];
        }

        std::vector<char> bytes() {
            std::vector<char> buff;
            buff.push_back(FEED_INIT_REP);
            buff.push_back(_token >> 24);
            buff.push_back((_token >> 16) & 0xff);
            buff.push_back((_token >> 8) & 0xff);
            buff.push_back(_token & 0xff);
            return buff;
        }

        char code() {
            return FEED_INIT_REP;
        }

        int32_t token() {
            return _token;
        }
    private:
        int32_t _token;
};
```

### User connect room (specifically `ROOM_CLIENT_CONNECT`)


The channel used is the `main` channel.

Sent when a client joins a room.

```
AAAABBBBBBBBBBBBB
 |       |
CODE   PLAYERID

CODE is the message code, it has the value of `ROOM_CLIENT_CONNECT` (refer to the relevant code table).
PLAYERID is the id of the player, it is encoded as an uint16_t.
```

### User disconnect from room (specifically `ROOM_CLIENT_DISCONNECT`)

The channel used is the `main` channel.

Sent when a client leaves the room.

```
AAAABBBBBBBBBBBBBCCCCCCCCCCC
 |       |            |
CODE  DCUSERID    NEWHOSTID

CODE is the message code, it has the value of `ROOM_CLIENT_DISCONNECT` (refer to the relevant code table).
DCUSERID is the id of the disconnected user.
NEWHOSTID is the new host of the game (might need clarification).
PLAYERID is the id of the player, it is encoded as an uint16_t.
```

### Request connect room (specifically `REQUEST_CONNECT_ROOM`)

The channel used is the `main` channel.

It is called to request for entering into a room.

```
AAAABBBBBBBBBBBBB
 |       |
CODE  ROOMID

CODE is the message code, it has the value of `REQUEST_CONNECT_ROOM` (refer to the relevant code table).
ROOMID is a multibyte room identifier
```

### Request connect room reply (specifically `CONNECT_ROOM_REQ_REP`)

The channel used is the `main` channel.

It is always sent in reply of a request connect room message.

```
AAAABBBBBBBBBB
 |       |
CODE  PLAYERID

CODE is the message code, it has the value of `CONNECT_ROOM_REQ_REP` (refer to the relevant code table).
PLAYERID is the ID of the player entering into the room.
```

### Create room reply (specifically `CREATE_ROOM_REPLY`)

The channel used is the `main` channel.

It is used in reply of a `CREATE_ROOM` signal marker.

```
AAAABBBBBBBBBB
 |       |
CODE  TOKEN

CODE is the message code, it has the value of `CONNECT_ROOM_REQ_REP` (refer to the relevant code table).
TOKEN is the token of the room.
```