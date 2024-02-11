cmake_minimum_required(VERSION 3.20)
project(pipes C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

add_executable(main
  ./main.c
)

add_executable(main_cv
  ./main_cv.c
)


target_link_libraries(main PRIVATE Threads::Threads)
target_link_libraries(main_cv PRIVATE Threads::Threads)
