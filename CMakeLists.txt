cmake_minimum_required(VERSION 3.10)

project(TicTacToeRL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(env_core src/environment.cpp)

# Test executables
add_executable(test_core_engine tests/test_core_engine.cpp)
target_link_libraries(test_core_engine env_core)

add_executable(test_state_representation tests/test_state_representation.cpp)
target_link_libraries(test_state_representation env_core)

add_executable(test_integration tests/test_integration.cpp)
target_link_libraries(test_integration env_core) 