cmake_minimum_required(VERSION 3.20)
project(files C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(1
  ./1.c
)

add_executable(2
  ./2.c
)
