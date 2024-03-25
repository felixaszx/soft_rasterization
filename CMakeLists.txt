cmake_minimum_required(VERSION 3.20.0)
project(project VERSION 0.1.0)

set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_C_EXTENSIONS off)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/exe)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_STATIC_LIBRARY_PREFIX "")

file(GLOB_RECURSE SRC src/*.cpp)
add_executable(project main.cpp ${SRC})

target_include_directories(project PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(project fltk)