cmake_minimum_required(VERSION 3.5)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CXXFLAGS "-Wall -Wextra -pedantic")

add_executable(client ./clientFolder/client.h ./clientFolder/start_client.cc ./clientFolder/client.cc)
add_executable(server ./serverFolder/server.h ./serverFolder/server_start.cc ./serverFolder/server.cc)