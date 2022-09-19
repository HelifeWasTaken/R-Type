project(rtype)
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(server)
add_subdirectory(client)
