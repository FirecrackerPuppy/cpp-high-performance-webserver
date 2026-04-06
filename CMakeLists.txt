cmake_minimum_required(VERSION 3.10)
project(cpp_webserver)

set(CMAKE_CXX_STANDARD 17)

add_executable(server
    src/main.cpp
)

target_link_libraries(server pthread)
