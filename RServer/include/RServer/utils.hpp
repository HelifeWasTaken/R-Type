#pragma once

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>

#include "RServer/async_queue.hpp"
#include "RServer/async_automated_sparse_array.hpp"
#include "RServer/utils.hpp"

#include <ostream>

namespace rtype {
namespace net {

#ifndef RTYPE_TCP_BUFFER_SIZE
#define RTYPE_TCP_BUFFER_SIZE 1024
#endif

#ifndef RTYPE_UDP_BUFFER_SIZE
#define RTYPE_UDP_BUFFER_SIZE 1024
#endif

    using tcp = boost::asio::ip::tcp;
    using udp = boost::asio::ip::udp;

    using tcp_buffer_t = boost::array<char, RTYPE_TCP_BUFFER_SIZE>;
    using udp_buffer_t = boost::array<char, RTYPE_UDP_BUFFER_SIZE>;

    static inline void dump_memory(std::ostream& os, const uint8_t *data, const size_t size)
    {
        os << "Size: " << size << " bytes: [";
        for (size_t i = 0; i < size; ++i) {
            os << "0x" << (uint32_t)data[i];
            if (i != size - 1)
                os << ", ";
        }
        os << "]";
    }

}
}