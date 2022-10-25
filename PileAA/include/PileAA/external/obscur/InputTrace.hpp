#pragma once

#include <vector>

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

namespace obscur {

template <typename T> class InputTrace {
    T _origin;
    std::vector<T> _followers;

public:
    InputTrace(const T& key) { _origin = key; }
    ~InputTrace() = default;

    T getOrigin() const { return _origin; }

    void addFollower(const T& key) { _followers.push_back(key); }

    void removeFollower(const T& key)
    {
        for (auto it = _followers.begin(); it != _followers.end(); it++) {
            if (*it == key) {
                _followers.erase(it);
                break;
            }
        }
    }

    bool checkCombination(const std::vector<T>& keys) const
    {
        if (keys.size() == 0)
            return false;
        if (keys.size() != _followers.size() + 1)
            return false;
        if (keys[0] != _origin)
            return false;
        for (size_t i = 1; i < keys.size(); i++) {
            if (keys[i] != _followers[i - 1])
                return false;
        }
        return true;
    }
};
}