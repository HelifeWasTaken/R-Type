#pragma once

#include "PileAA/meta.hpp"

#include <queue>
#include <mutex>

namespace rtype {
namespace net {

    template <typename T> class async_queue : public std::queue<T> {
    public:
        HL_AUTO_COMPLETE_CANONICAL_FORM(async_queue);

        bool async_empty()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return this->empty();
        }

        bool async_pop(T& value)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (this->empty())
                return false;
            value = std::move(this->front());
            this->pop();
            return true;
        }

        void async_push(const T& value)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            this->push(value);
        }

        void async_push(T&& value)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            this->push(std::move(value));
        }

        template <typename... Args> void async_emplace(Args&&... args)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            this->emplace(std::forward<Args>(args)...);
        }

    private:
        std::mutex _mutex;
    };

}
}