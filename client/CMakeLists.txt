project(r-type_client)

set(SRC main.cpp)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} sfml-system sfml-audio sfml-graphics sfml-window)
