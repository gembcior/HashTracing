cmake_minimum_required(VERSION 3.24)

project(hashTracing CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

add_executable(hash)

target_include_directories(hash
  PUBLIC
    inc
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(src)
