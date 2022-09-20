#pragma once

#include <utility>
#include <string>
#include <mutex>

#define HL_FACTORY_IMPL(T, name) \
template<typename T> \
struct name { \
template<typename U, typename ...Args> \
static T *create(Args&& ...args) { return new U(std::forward<Args>(args)...); } \
};

#define HL_SINGLETON_IMPL(T, name) \
class name { \
protected: \
    static inline T *_impl = nullptr; \
    static inline std::mutex _mut; \
public: \
    static inline T& get() { \
        std::lock_guard<std::mutex> _(_mut); \
        if (!_impl) \
            _impl = new T; \
        return *_impl; \
    } \
    static inline void release() { \
        std::lock_guard<std::mutex> _(_mut); \
        if (!_impl) \
            return; \
        delete _impl; \
        _impl = nullptr; \
    } \
};

#define HL_BASE_ERROR_IMPL(name) \
class name { \
private: \
    const std::string _msg; \
public: \
    name(const std::string& msg) \
        : _msg(msg) {} \
    virtual const char *what() const \
    { return _msg.c_str(); } \
};

#define HL_SUB_ERROR_IMPL(name, base) \
struct name : public base { \
    name(const std::string& msg) \
        : base(msg) {} \
};
