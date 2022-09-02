cmake_minimum_required(VERSION 2.8)
project (cdl-ex)


include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(my-ex main.cpp)
target_link_libraries(my-ex pthread)
