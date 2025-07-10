cmake_minimum_required(VERSION 3.10)

project(TicTacToeRL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(env_core src/environment.cpp)

add_executable(env_test tests/env_test.cpp)
target_link_libraries(env_test env_core) 