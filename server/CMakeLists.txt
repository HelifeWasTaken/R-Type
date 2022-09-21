project(r-type_server)

file(GLOB_RECURSE SRC *.cpp *.hpp)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} -lboost_system -lboost_thread -lpthread)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
