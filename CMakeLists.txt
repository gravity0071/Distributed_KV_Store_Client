cmake_minimum_required(VERSION 3.10)

project(Distributed_KV_Store_Client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 包含 util 文件夹路径
include_directories(${CMAKE_SOURCE_DIR}/util)

# 查找 util 文件夹中的所有 .cpp 文件
file(GLOB UTIL_SOURCES "util/*.cpp")

# 设置可执行文件
add_executable(MyExecutable main.cpp ${UTIL_SOURCES}
        util/ClientToServer.h
        util/ClientToServer.cpp)

# 添加 nlohmann/json 库
include(FetchContent)
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.10.5
)
FetchContent_MakeAvailable(nlohmann_json)

target_link_libraries(MyExecutable PRIVATE nlohmann_json::nlohmann_json)
