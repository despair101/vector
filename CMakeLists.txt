cmake_minimum_required(VERSION 3.28)
project(vector)

set(CMAKE_CXX_STANDARD 20)
find_package(Catch2 REQUIRED)

add_executable(test tests/test.cpp)
target_link_libraries(test PRIVATE Catch2::Catch2WithMain)
