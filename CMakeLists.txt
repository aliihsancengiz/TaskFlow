cmake_minimum_required(VERSION 3.16)
project("TaskFlow" VERSION 0.1.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(example)
add_subdirectory(tests)