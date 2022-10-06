project(r-type_client)

link_libraries(sfml-system sfml-audio sfml-graphics sfml-window boost_system boost_thread)

file(GLOB_RECURSE SRC *.cpp *.hpp)

add_library(${PROJECT_NAME} STATIC ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
# project(r-type_client)

# file(GLOB_RECURSE SRC *.cpp *.hpp)

# add_executable(${PROJECT_NAME} ${SRC})

# target_link_libraries(${PROJECT_NAME} sfml-system sfml-audio sfml-graphics sfml-window boost_system boost_thread)
# target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC "/ZI")
    target_link_options(${PROJECT_NAME} PUBLIC "/INCREMENTAL")
endif()