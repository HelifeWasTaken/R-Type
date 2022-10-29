#pragma once

#include <mutex>
#include <string>
#include <utility>

/**
 * @brief Generate a base factory class
 *
 */
#define HL_FACTORY_IMPL(T, name)                                               \
    template <typename T> struct name {                                        \
        template <typename U, typename... Args>                                \
        static T* create(Args&&... args)                                       \
        {                                                                      \
            return new U(std::forward<Args>(args)...);                         \
        }                                                                      \
    };

/**
 * @brief Generates a Singleton class from a type
 *
 */
#define HL_SINGLETON_IMPL(T, name)                                             \
    class name {                                                               \
    protected:                                                                 \
        static inline T* _impl = nullptr;                                      \
        static inline std::mutex _mut;                                         \
                                                                               \
    public:                                                                    \
        static inline T& get()                                                 \
        {                                                                      \
            std::lock_guard<std::mutex> _(_mut);                               \
            if (!_impl)                                                        \
                _impl = new T;                                                 \
            return *_impl;                                                     \
        }                                                                      \
        static inline void release()                                           \
        {                                                                      \
            std::lock_guard<std::mutex> _(_mut);                               \
            if (!_impl)                                                        \
                return;                                                        \
            delete _impl;                                                      \
            _impl = nullptr;                                                   \
        }                                                                      \
    };

#define HL_ERROR_IMPL_FROM_STD_EXCEPTION(name)                                 \
    class name : public std::exception {                                       \
    protected:                                                                 \
        const std::string _msg;                                                \
                                                                               \
    public:                                                                    \
        name(const std::string& msg)                                           \
            : _msg(msg)                                                        \
        {                                                                      \
        }                                                                      \
        name(const char* msg)                                                  \
            : _msg(msg)                                                        \
        {                                                                      \
        }                                                                      \
        const char* what() const noexcept override { return _msg.c_str(); }    \
    };

/**
 * @brief Generates an error class from a name
 *
 */
#define HL_BASE_ERROR_IMPL(name)                                               \
    class name {                                                               \
    private:                                                                   \
        const std::string _msg;                                                \
                                                                               \
    public:                                                                    \
        name(const std::string& msg)                                           \
            : _msg(msg)                                                        \
        {                                                                      \
        }                                                                      \
        virtual const char* what() const { return _msg.c_str(); }              \
    };

/**
 * @brief Generate a subclass of an error class
 * (That is compliant with HL_BASE_ERROR_IMPL)
 *
 */
#define HL_SUB_ERROR_IMPL(name, base)                                          \
    struct name : public base {                                                \
        name(const std::string& msg)                                           \
            : base(msg)                                                        \
        {                                                                      \
        }                                                                      \
    };

/**
 * @brief Automatically complete the canonical form of a class
 *
 */
#define HL_AUTO_COMPLETE_CANONICAL_FORM(tname)                                 \
    tname() = default;                                                         \
    tname(const tname&) = default;                                             \
    tname(tname&&) = default;                                                  \
    tname& operator=(const tname&) = default;                                  \
    tname& operator=(tname&&) = default;                                       \
    ~tname() = default;
