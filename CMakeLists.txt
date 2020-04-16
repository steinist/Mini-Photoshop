cmake_minimum_required(VERSION 3.15)
project(Assignment3)

set(CMAKE_CXX_STANDARD 11)
set(GCC_COVERAGE_COMPILE_FLAGS "-static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

add_executable(Assignment3 main.cpp canvas.cpp patch.cpp repl.cpp utils.cpp)

