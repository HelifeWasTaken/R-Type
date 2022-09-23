project(rtype-poc)

file(GLOB_RECURSE SRC *.cpp *.hpp)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})