#pragma once

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>

#include "RServer/async_queue.hpp"
#include "RServer/async_automated_sparse_array.hpp"

namespace rtype {
namespace net {

#ifndef RTYPE_TCP_BUFFER_SIZE
    #define RTYPE_TCP_BUFFER_SIZE 200
#endif

#ifndef RTYPE_UDP_BUFFER_SIZE
    #define RTYPE_UDP_BUFFER_SIZE 200
#endif

    using tcp = boost::asio::ip::tcp;
    using udp = boost::asio::ip::udp;

    using ClientID = uint16_t;

#ifndef RTYPE_MAX_SERVER_CONNECTIONS
    #define RTYPE_MAX_SERVER_CONNECTIONS (std::numeric_limits<ClientID>::max() - 1)
#endif

#ifndef RTYPE_PLAYER_COUNT
    #define RTYPE_PLAYER_COUNT 4
#endif

#ifndef RTYPE_CONN_INIT_REPLY_TOKEN
    #define RTYPE_CONN_INIT_REPLY_TOKEN 42
#endif

#ifndef RTYPE_FEED_INIT_TOKEN
    #define RTYPE_FEED_INIT_TOKEN 84
#endif

#if defined(USE_RTYPE_MAGIC_NUMBER_64_BITS) + defined(USE_RTYPE_MAGIC_NUMBER_32_BITS) + defined(USE_RTYPE_MAGIC_NUMBER_16_BITS) + defined(USE_RTYPE_MAGIC_NUMBER_8_BITS) != 1
    #error "You must define one and only one of the following macros: USE_RTYPE_MAGIC_NUMBER_64_BITS, USE_RTYPE_MAGIC_NUMBER_32_BITS, USE_RTYPE_MAGIC_NUMBER_16_BITS, USE_RTYPE_MAGIC_NUMBER_8_BITS"
#endif

#if defined(USE_RTYPE_MAGIC_NUMBER_64_BITS)
    #define RTYPE_MAGIC_NUMBER 0xff1cec0ffeedefec
    using MagicNumber = uint64_t;
#elif defined(USE_RTYPE_MAGIC_NUMBER_32_BITS)
    #define RTYPE_MAGIC_NUMBER 0xff1cec0f
    using MagicNumber = uint32_t;
#elif defined(USE_RTYPE_MAGIC_NUMBER_16_BITS)
    #define RTYPE_MAGIC_NUMBER 0xff1c
    using MagicNumber = uint16_t;
#elif defined(USE_RTYPE_MAGIC_NUMBER_8_BITS)
    #define RTYPE_MAGIC_NUMBER 0xff
    using MagicNumber = uint8_t;
#endif

    using PlayerID = uint8_t;

    using TokenType = uint8_t;
    using BufferSizeType = size_t;
    using Byte = uint8_t;
    using PortType = uint16_t;
    using Bool = bool;

    using tcp_buffer_t = boost::array<Byte, RTYPE_TCP_BUFFER_SIZE>;
    using udp_buffer_t = boost::array<Byte, RTYPE_UDP_BUFFER_SIZE>;

    static inline void dump_memory(const Byte *data, const BufferSizeType size)
    {
        std::printf("Dumping memory at %p of size %lu\n", data, size);
        std::printf("bytes: [");
        for (BufferSizeType i = 0; i < size; ++i) {
            std::printf("0x%hhx", data[i]);
            if (i != size - 1)
                std::printf(", ");
        }
        std::printf("]\n");
    }

    template<typename T>
    static inline void dump_memory(const T& data)
    {
        dump_memory(reinterpret_cast<const Byte*>(&data), sizeof(T));
    }

    template<typename T>
    static inline void dump_memory(const std::vector<T>& data)
    {
        dump_memory(reinterpret_cast<const Byte*>(data.data()), data.size() * sizeof(T));
    }

}
}