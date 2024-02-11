cmake_minimum_required(VERSION 3.20)
project(messages C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(client
  ./client.c
)

add_executable(server
  ./server.c
)

add_executable(test_client
  ./test_client.c
)

add_executable(test_server
  ./test_server.c
)
