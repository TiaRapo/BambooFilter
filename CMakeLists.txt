cmake_minimum_required(VERSION 3.10)
project(BambooFilterProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIBRARY_SOURCES
    src/bamboo_filter.cc
    src/segment.cc
    src/bucket.cc
)

add_executable(test_file
    ${LIBRARY_SOURCES}
    tests/test_file.cc
)

add_executable(test_random
    ${LIBRARY_SOURCES}
    tests/test_random.cc
)

target_include_directories(test_file PRIVATE
    include    
    third_party
)

target_include_directories(test_random PRIVATE
    include
    third_party
)

target_compile_options(test_file PRIVATE
    $<$<CONFIG:Release>:-O3 -march=native>
    $<$<CONFIG:Debug>:-O0 -g>
)

target_compile_options(test_random PRIVATE
    $<$<CONFIG:Release>:-O3 -march=native>
    $<$<CONFIG:Debug>:-O0 -g>
)

set_property(TARGET test_file PROPERTY CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
set_property(TARGET test_random PROPERTY CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)