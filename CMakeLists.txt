project(rtype)
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

include_directories(${SFML_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(server)
add_subdirectory(client)
