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
0000000AAAAAAAABBBBBBBBBCCCCCCCCCCDDDDDDDDDDDDDDDDDD
 |       |        |         |           |
MAGIC   SEQ      FROM        TO       CONTENT

MAGIC is a magic number, used to ensure that the message is valid. its value is 0x0fficecoffeedefec.
SEQ is the sequence number, encoded as an int64, it is incremented by one for each message sent.
FROM is the sender ID, encoded as a int16, used to identify the host that sent the message.
TO is the target ID, encoded as a int16, used to identify the host that should receive the message.
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
    private.
        char _code;
};
```

### Update message (specifically `UPDATE_MSG`)

The channel used is the `feed` channel.

```
AAAABBBBBBBBCCCCCCCCCCCC
 |      |         |
CODE   SID       DATA

CODE is the message code, it has the value of `UPDATE_MSG` (refer to the relevant code table).
SID is the shared identifier, it should refer to the same element for the server and for every client.
DATA is the serialized data which is relevant for this SID. its size in bytes is unspecified and can depend on the relevant SID.
```

Sample implementation:
```cpp

class UpdateMessage : public IMessage {
    public:
        UpdateMessage() : _sid(0) {}
        UpdateMessage(int16_t sid, const std::vector<char>& data) : _sid(sid), _data(data) {}

        void from(const std::vector<char>& buff) {
            _sid = (buff[0] << 8) | buff[1];
            _data = std::vector<char>(buff.begin() + 2, buff.end());
        }

        std::vector<char> bytes() {
            std::vector<char> buff;
            buff.push_back(UPDATE_MSG);
            buff.push_back(_sid >> 8);
            buff.push_back(_sid & 0xff);
            buff.insert(buff.end(), _data.begin(), _data.end());
            return buff;
        }

        char code() {
            return UPDATE_MSG;
        }

        int16_t sid() {
            return _sid;
        }

        std::vector<char> data() {
            return _data;
        }
    private:
        int16_t _sid;
        std::vector<char> _data;
};
```

### Synchronization message (specifically `SYNC_MSG`)

The channel used is the `main` channel.

```
AAAABBBBBBBBCCCCCCCCCCCC
 |      |         |
CODE   SID       DATA

CODE is the message code, it has the value of `SYNC_MSG` (refer to the relevant code table).
SID is the shared identifier, it should refer to the same element for the server and for every client.
DATA is the serialized data which is relevant for this SID. its size in bytes is unspecified and can depend on the relevant SID.
```

Sample implementation:
```cpp

class SyncMessage : public IMessage {
    public:
        SyncMessage() : _sid(0) {}
        SyncMessage(int16_t sid, const std::vector<char>& data) : _sid(sid), _data(data) {}

        void from(const std::vector<char>& buff) {
            _sid = (buff[0] << 8) | buff[1];
            _data = std::vector<char>(buff.begin() + 2, buff.end());
        }

        std::vector<char> bytes() {
            std::vector<char> buff;
            buff.push_back(SYNC_MSG);
            buff.push_back(_sid >> 8);
            buff.push_back(_sid & 0xff);
            buff.insert(buff.end(), _data.begin(), _data.end());
            return buff;
        }

        char code() {
            return SYNC_MSG;
        }

        int16_t sid() {
            return _sid;
        }

        std::vector<char> data() {
            return _data;
        }
    private:
        int16_t _sid;
        std::vector<char> _data;
};
```

### Query message (specifically `QUERY_MSG`)

The channel used can be either the `main` channel or the `feed` channel.

```
AAAABBBBBBBBB
 |      |
CODE   DATA

CODE is the message code, it has the value of a message code which is of query type (refer to the relevant code table).
DATA is the serialized data which is relevant for this query. its size in bytes is defined according to the specified CODE.
```

Sample implementation:
```cpp

class QueryMessage : public IMessage {
    public:
        QueryMessage() : _code(0) {}
        QueryMessage(char code, const std::vector<char>& data) : _code(code), _data(data) {}

        void from(const std::vector<char>& buff) {
            _code = buff[0];
            _data = std::vector<char>(buff.begin() + 1, buff.end());
        }

        std::vector<char> bytes() {
            std::vector<char> buff;
            buff.push_back(_code);
            buff.insert(buff.end(), _data.begin(), _data.end());
            return buff;
        }

        char code() {
            return _code;
        }

        std::vector<char> data() {
            return _data;
        }
    private:
        char _code;
        std::vector<char> _data;
};
```

### Reply message

The channel used can be either the `main` channel or the `feed` channel.

A reply message cannot and should not be sent without having a query message received before.

```
AAAABBBBBCCCCCCCCCC
 |    |       |
CODE STATUS  DATA

CODE is the message code, it has the value of a message code which is of reply type (refer to the relevant code table).
STATUS is the status code, it has the value of any status code (refer to the relevant code table).
DATA is the serialized data which is relevant for this reply. its size in bytes is defined according to the specified CODE.
```

Sample implementation:
```cpp

class ReplyMessage : public IMessage {
    public:
        ReplyMessage() : _code(0), _status(0) {}
        ReplyMessage(char code, char status, const std::vector<char>& data) : _code(code), _status(status), _data(data) {}

        void from(const std::vector<char>& buff) {
            _code = buff[0];
            _status = buff[1];
            _data = std::vector<char>(buff.begin() + 2, buff.end());
        }

        std::vector<char> bytes() {
            std::vector<char> buff;
            buff.push_back(_code);
            buff.push_back(_status);
            buff.insert(buff.end(), _data.begin(), _data.end());
            return buff;
        }

        char code() {
            return _code;
        }

        char status() {
            return _status;
        }

        std::vector<char> data() {
            return _data;
        }
    private:
        char _code;
        char _status;
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
AAAABBBBBBBBBBCCCCCCCCCCCCC
 |      |         |
CODE PLAYERID   TOKEN

CODE is the message code, it has the value of `FEED_INIT` (refer to the relevant code table).
PLAYERID is the player ID, it is encoded as an int16.
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