include(FetchContent)

message("Declaring lua")
FetchContent_Declare(
    lua
    GIT_REPOSITORY "https://github.com/marovira/lua"
    GIT_TAG a32ae8de013ebe1feb3c0d22d1028ff5d2617f84
)

message("Getting lua properties...")
FetchContent_GetProperties(lua)
if (NOT lua_POPULATED)
    message("Populating lua...")
    FetchContent_Populate(lua)
endif()

message("Fetching lua")
FetchContent_MakeAvailable(lua)
