cmake_minimum_required(VERSION 3.1)

project(practice_one)

# To disable runtime checks, remove "-fsanitize=address" below
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++1y -Wall -Wextra")

add_executable(practice_one main.cpp)
