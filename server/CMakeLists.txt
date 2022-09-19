project(r-type_server)

#file(GLOB_RECURSE SRC src/*.cpp)
set(SRC main.cpp)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})
