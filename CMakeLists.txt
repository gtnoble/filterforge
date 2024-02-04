cmake_minimum_required(VERSION 3.20)
project(filterforge)

include(CTest)

add_compile_options(-g -Wall -Wpedantic -Wextra -Werror -std=c2x)
include_directories(include)
add_subdirectory(src)
add_subdirectory(test)