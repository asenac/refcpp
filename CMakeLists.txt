cmake_minimum_required(VERSION 2.8)
project(ref CXX)

set(CMAKE_CXX_FLAGS "-Wall -Werror -std=c++11")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

enable_testing()

add_subdirectory(ref)
add_subdirectory(examples)
add_subdirectory(test)

