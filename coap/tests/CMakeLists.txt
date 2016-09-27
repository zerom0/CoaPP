cmake_minimum_required(VERSION 2.8.7)
project(CoaP)

file(GLOB_RECURSE SRCS *.cpp)

include_directories(
        ../include
        ../src
)

add_executable(testsuite ${SRCS})
#target_include_directories(testsuite PUBLIC include src)
target_link_libraries(testsuite coap gtest_main)

add_test(NAME testsuite COMMAND testsuite)
