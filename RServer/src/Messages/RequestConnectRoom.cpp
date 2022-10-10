namespace rtype {
namespace net {

    RequestConnectRoom(const std::string& roomID)
        : Message(message_code::REQUEST_CONNECT_ROOM)
        , _roomID(roomID)
    {
    }

    void from(const uint8_t *data, const size_t size) override
    {
        Serializer s(data, size);
        s >> _message_code;
        _roomID = std::string(s.data.begin(), s.data.end());
    }

    std::vector<uint8_t> serialize() const override
    {
        Serializer s;

        s << _message_code;
        s.add_bytes(_roomID);
        return s.data;
    }

}
}