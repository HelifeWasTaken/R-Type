project(rtype-poc)

file(GLOB_RECURSE SRC *.cpp *.hpp)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME}
    sfml-graphics sfml-window sfml-system
    boost_system boost_thread
    pileaa
    rserver)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
