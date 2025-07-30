cmake_minimum_required(VERSION 3.26.0)
project(glslx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(json)
add_subdirectory(glslang)
add_subdirectory(src)
