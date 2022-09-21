project(rtype)
cmake_minimum_required(VERSION 3.15)

add_compile_definitions(SILVA_STATE_DRAW)
add_compile_definitions(SILVA_STATE_HANDLE_EVENT)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

include(FetchContent)

FetchContent_Declare(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.6.x)
FetchContent_Declare(BOOST_ASIO GIT_REPOSITORY https://github.com/boostorg/asio.git)
FetchContent_MakeAvailable(BOOST_ASIO)
FetchContent_MakeAvailable(SFML)

include_directories(
    ${SFML_SOURCE_DIR}/include
    ${BOOST_ASIO_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
)

add_subdirectory(POC)
add_subdirectory(server)
add_subdirectory(client)