#pragma once

#include <cstring>
#include <vector>
#include "RServer/Messages/Messages.hpp"
#include "PileAA/meta.hpp"

namespace rtype {
namespace net {

    enum class serializable_type : int8_t {
        VECTOR2I
    };

    struct vector2i : public Serializable {
        int x = 0;
        int y = 0;

        vector2i() : Serializable((int8_t)serializable_type::VECTOR2I) {}

        vector2i(int x, int y)
            : Serializable((int8_t)serializable_type::VECTOR2I)
            , x(x) , y(y)
        {}

        vector2i(const vector2i& other)
            : Serializable((int8_t)serializable_type::VECTOR2I)
            , x(other.x) , y(other.y)
        {}

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
    };

}
}