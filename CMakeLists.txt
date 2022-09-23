project(rtype)
cmake_minimum_required(VERSION 3.15)

add_compile_definitions(SILVA_STATE_DRAW)
add_compile_definitions(SILVA_STATE_HANDLE_EVENT)

add_compile_definitions(_WIN32_WINNT=0x0601)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

include(FetchContent)

message("Declaring SFML...")
FetchContent_Declare(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.6.x)

message("Fetching SFML...")
FetchContent_MakeAvailable(SFML)

message("Done fetching dependencies.")

include_directories(
    ${SFML_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
)

add_subdirectory(POC)
add_subdirectory(server)
add_subdirectory(client)