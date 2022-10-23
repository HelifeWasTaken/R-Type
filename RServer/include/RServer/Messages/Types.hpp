#pragma once

#include <cstring>
#include <vector>
#include "RServer/Messages/Messages.hpp"
#include "PileAA/meta.hpp"

namespace rtype {
namespace net {

    enum class serializable_type : Byte {
        VECTOR2I
    };

    struct vector2i : public Serializable {
        uint32_t x = 0;
        uint32_t y = 0;

        vector2i() = default;

        vector2i(int x, int y)
            : Serializable((int8_t)serializable_type::VECTOR2I)
            , x(x) , y(y)
        {}

        vector2i(const vector2i& other)
            : Serializable((int8_t)serializable_type::VECTOR2I)
            , x(other.x) , y(other.y)
        {}

        vector2i(const std::vector<Byte>& data)
            : Serializable((int8_t)serializable_type::VECTOR2I)
        {
            from(data.data(), data.size());
        }

        vector2i& operator=(const vector2i& other) = default;

        std::vector<Byte> serialize() const override
        {
            Serializer s;
            s << _type << x << y;
            return s.data;
        }

        void from(const Byte *data, const BufferSizeType size) override
        {
            Serializer s(data, size);
            s >> _type >> x >> y;
        }
    };

}
}