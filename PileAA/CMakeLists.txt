project(pileaa)

# set(CMAKE_BUILD_TYPE Debug)

link_libraries(sfml-graphics sfml-window sfml-system sfml-audio)

file(GLOB_RECURSE SRC *.cpp *.hpp)

add_library(${PROJECT_NAME} STATIC ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
