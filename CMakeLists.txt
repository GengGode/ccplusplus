cmake_minimum_required(VERSION 3.15)

project(ccpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_subdirectory(source)
add_executable(ccpp.test source/main.cpp)