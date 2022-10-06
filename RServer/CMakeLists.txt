project(rserver)

link_libraries(boost_system boost_thread)

file(GLOB_RECURSE SRC *.cpp *.hpp)

add_library(${PROJECT_NAME} STATIC ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
