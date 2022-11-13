#include "RServer/Messages/Messages.hpp"

namespace rtype {
namespace net {

    GameLauncher::GameLauncher(int32_t seed, bool yes)
        : Message(message_code::LAUNCH_GAME_REP)
        , _seed(seed)
        , _yes(yes)
    {}

    void GameLauncher::from(const Byte* data, const BufferSizeType size)
    {
        Serializer s(data, size);
        s >> _message_code >> _yes >> _seed;
    }

    std::vector<Byte> GameLauncher::serialize() const
    {
        Serializer s;
        s << _message_code << _yes << _seed;
        return s.data;
    }

    BufferSizeType GameLauncher::size() const
    {
        return sizeof(_message_code) + sizeof(_yes) + sizeof(_seed);
    }

    bool GameLauncher::yes() const { return static_cast<bool>(_yes); }

    int32_t GameLauncher::seed() const { return _seed; }

}
}
