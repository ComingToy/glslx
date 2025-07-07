cmake_minimum_required(VERSION 3.26.0)
project(glsld)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(json)
add_subdirectory(glslang)

add_executable(compile_compute_shader compile_compute_shader.cc) 
target_link_libraries(compile_compute_shader PUBLIC MachineIndependent)
target_include_directories(compile_compute_shader PUBLIC glslang)

add_library(lsp STATIC protocol.cc completion.cc doc.cc workspace.cc parser.cc)
target_link_libraries(lsp PUBLIC MachineIndependent nlohmann_json::nlohmann_json)
target_include_directories(lsp PUBLIC json/include/)
target_include_directories(lsp PUBLIC glslang)

add_executable(glsld glsld.cc)
add_executable(test_protocol test_protocol.cc)
target_link_libraries(glsld PUBLIC lsp)
target_link_libraries(test_protocol PUBLIC lsp)
