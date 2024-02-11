cmake_minimum_required(VERSION 3.20)
project(pipes C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(client
  ./client.c
)

add_executable(server
  ./server.c
)
