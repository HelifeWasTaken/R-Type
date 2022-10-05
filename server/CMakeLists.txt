project(r-type_server)

file(GLOB_RECURSE SRC *.cpp *.hpp)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} boost_system boost_thread)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC "/ZI")
    target_link_options(${PROJECT_NAME} PUBLIC "/INCREMENTAL")
endif()
