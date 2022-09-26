project(rtype)
cmake_minimum_required(VERSION 3.15)

add_compile_definitions(SILVA_STATE_DRAW)
add_compile_definitions(SILVA_STATE_HANDLE_EVENT)

add_compile_definitions(_WIN32_WINNT=0x0601)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

include(FetchContent)

message("Declaring SFML...")
FetchContent_Declare(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

if (NOT hl_FOUND)
	FetchContent_Declare(
		hl
		GIT_REPOSITORY https://github.com/HelifeWasTaken/hl
	)
	FetchContent_GetProperties(hl)
	if (NOT hl_POPULATED)
		FetchContent_Populate(hl)
		set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
		set(hl_FOUND TRUE)
	endif()
endif()

include_directories(
	${SFML_SOURCE_DIR}/include
	${BOOST_ASIO_SOURCE_DIR}/include
	${hl_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/include
)

#add_subdirectory(server)

add_compile_definitions(RTYPE_BUILDING_CLIENT)
add_compile_definitions(RTYPE_USING_POC_MAIN)

add_subdirectory(POC)
add_subdirectory(server)
add_subdirectory(client)
