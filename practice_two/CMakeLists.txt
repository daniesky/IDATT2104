cmake_minimum_required(VERSION 3.1)

project(worker_threads)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++1y -Wall -Wextra")

add_executable(worker_threads main.cpp)
