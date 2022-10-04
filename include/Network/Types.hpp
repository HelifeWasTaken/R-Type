#pragma once

#include <cstring>
#include <vector>

namespace rtype {
namespace net {

enum serializable_type {
    VECTOR2F,
    VECTOR2I,
};

struct vector2f {
    float x;
    float y;

    vector2f(float x, float y) : x(x), y(y) {}

    vector2f() : x(0), y(0) {}

    vector2f(const vector2f& other) : x(other.x), y(other.y) {}

    vector2f& operator=(const vector2f& other) {
        x = other.x;
        y = other.y;
        return *this;
    }

    static serializable_type get_serializable_type() { return serializable_type::VECTOR2F; }

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> buffer(2 * sizeof(float));
        std::memcpy(buffer.data(), &x, sizeof(float));
        std::memcpy(buffer.data() + sizeof(float), &y, sizeof(float));
        return buffer;
    }

    static vector2f deserialize(const std::vector<uint8_t>& buffer) {
        vector2f vec;
        std::memcpy(&vec.x, buffer.data(), sizeof(float));
        std::memcpy(&vec.y, buffer.data() + sizeof(float), sizeof(float));
        return vec;
    }
};

struct vector2i {
    int x;
    int y;

    vector2i(int x, int y) : x(x), y(y) {}

    vector2i() : x(0), y(0) {}

    vector2i(const vector2i& other) : x(other.x), y(other.y) {}

    vector2i& operator=(const vector2i& other) {
        x = other.x;
        y = other.y;
        return *this;
    }

    static serializable_type get_serializable_type() { return serializable_type::VECTOR2I; }

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> buffer(2 * sizeof(int));
        std::memcpy(buffer.data(), &x, sizeof(int));
        std::memcpy(buffer.data() + sizeof(int), &y, sizeof(int));
        return buffer;
    }

    static vector2i deserialize(const std::vector<uint8_t>& buffer) {
        vector2i vec;
        std::memcpy(&vec.x, buffer.data(), sizeof(int));
        std::memcpy(&vec.y, buffer.data() + sizeof(int), sizeof(int));
        return vec;
    }
};

}
}