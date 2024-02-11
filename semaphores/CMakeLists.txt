cmake_minimum_required(VERSION 3.20)
project(messages C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(semaphores
  ./semaphores.c
)
