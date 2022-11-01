include(FetchContent)

message("Declaring lua")
FetchContent_Declare(
    lua
    GIT_REPOSITORY "https://github.com/marovira/lua"
    GIT_TAG a32ae8de013ebe1feb3c0d22d1028ff5d2617f84
)

message("Fetching lua")
FetchContent_MakeAvailable(lua)
