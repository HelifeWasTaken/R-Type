project(pileaa)

set(CMAKE_BUILD_TYPE Debug)

link_libraries(sfml-graphics sfml-window sfml-system sfml-audio)

if (WIN32)
    if(CMAKE_BUILD_TYPE EQUAL "Release")
        link_libraries(${lua_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lua_lib.lib)
    else()
        link_libraries(${lua_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lua_libd.lib)
    endif()
else()
    link_libraries(lua)
endif()

file(GLOB_RECURSE SRC *.cpp *.hpp)

add_library(${PROJECT_NAME} STATIC ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
