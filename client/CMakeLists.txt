project(r-type_client)

file(GLOB_RECURSE SRC *.cpp *.hpp)

add_executable(${PROJECT_NAME} ${SRC})

if(MSVC)
    if($<CONFIG:Release>)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SFML_BINARY_DIR}/lib/$<CONFIG>/sfml-system-d-2.dll ${PROJECT_BINARY_DIR}/$<CONFIG>)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SFML_BINARY_DIR}/lib/$<CONFIG>/sfml-audio-d-2.dll ${PROJECT_BINARY_DIR}/$<CONFIG>)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SFML_BINARY_DIR}/lib/$<CONFIG>/sfml-graphics-d-2.dll ${PROJECT_BINARY_DIR}/$<CONFIG>)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SFML_BINARY_DIR}/lib/$<CONFIG>/sfml-window-d-2.dll ${PROJECT_BINARY_DIR}/$<CONFIG>)
    else()
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SFML_BINARY_DIR}/lib/$<CONFIG>/sfml-system-2.dll ${PROJECT_BINARY_DIR}/$<CONFIG>)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SFML_BINARY_DIR}/lib/$<CONFIG>/sfml-audio-2.dll ${PROJECT_BINARY_DIR}/$<CONFIG>)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SFML_BINARY_DIR}/lib/$<CONFIG>/sfml-graphics-2.dll ${PROJECT_BINARY_DIR}/$<CONFIG>)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SFML_BINARY_DIR}/lib/$<CONFIG>/sfml-window-2.dll ${PROJECT_BINARY_DIR}/$<CONFIG>)
    endif()
        
endif()

target_link_libraries(${PROJECT_NAME} sfml-system sfml-audio sfml-graphics sfml-window)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
