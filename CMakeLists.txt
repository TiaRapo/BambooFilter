cmake_minimum_required(VERSION 3.10)
project(BambooFilterProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

add_compile_options(-O3 -march=native)

include_directories(include)

set(SOURCES
    src/test.cc # TODO: Remove test.cc from sources
    src/bamboo_filter.cc
    src/segment.cc
    src/utility.cc
)

add_executable(test ${SOURCES})