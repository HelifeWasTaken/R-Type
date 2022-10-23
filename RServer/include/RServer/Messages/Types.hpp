#pragma once

#include <cstring>
#include <vector>
#include "RServer/Messages/Messages.hpp"
#include "PileAA/meta.hpp"

namespace rtype {
namespace net {

    struct vector2i : public Serializable {
        uint32_t x = 0;
        uint32_t y = 0;

        vector2i() = default;

        vector2i(int x, int y)
            : x(x) , y(y)
        {}

        vector2i(const vector2i& other)
            : x(other.x) , y(other.y)
        {}

        vector2i(const std::vector<uint8_t>& data)
        {
            from(data.data(), data.size());
        }

        vector2i(const uint8_t *data, const size_t size)
        {
            from(data, size);
        }

        vector2i& operator=(const vector2i& other) = default;

        std::vector<uint8_t> serialize() const override
        {
            Serializer s;
            s << x << y;
            return s.data;
        }

        void from(const uint8_t *data, const size_t size) override
        {
            Serializer s(data, size);
            s >> x >> y;
        }

        bool operator==(const vector2i& other) const
        {
            return x == other.x && y == other.y;
        }

        bool operator!=(const vector2i& other) const
        {
            return !(*this == other);
        }
    };

    struct srand_sync : public Serializable {
        int32_t seed = std::time(nullptr);

        srand_sync()
            : seed(std::time(nullptr))
        {}

        srand_sync(int32_t seed)
            : seed(seed)
        {}

        srand_sync(const std::vector<uint8_t>& data)
        {
            from(data.data(), data.size());
        }

        srand_sync(const uint8_t *data, const size_t size)
        {
            from(data, size);
        }

        std::vector<uint8_t> serialize() const override
        {
            Serializer s;
            s << seed;
            return s.data;
        }

        void from(const uint8_t *data, const size_t size) override
        {
            Serializer s(data, size);
            s >> seed;
        }
    };

}
}
