cmake_minimum_required(VERSION 3.20)
project("river gateway" C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)
find_package(Curses REQUIRED)

add_executable(gateway
  ./gateway.c
  ./globals.h
)

add_executable(upstream
  ./upstream.c
  ./globals.h
)

add_executable(downstream
  ./downstream.c
  ./globals.h
)

target_link_libraries(gateway PRIVATE Threads::Threads ${CURSES_LIBRARIES})
target_link_libraries(upstream PRIVATE Threads::Threads ${CURSES_LIBRARIES})
target_link_libraries(downstream PRIVATE Threads::Threads ${CURSES_LIBRARIES})

target_include_directories(gateway PRIVATE ${CURSES_INCLUDE_DIR})
target_include_directories(upstream PRIVATE ${CURSES_INCLUDE_DIR})
target_include_directories(downstream PRIVATE ${CURSES_INCLUDE_DIR})
