cmake_minimum_required(VERSION 2.8.7)
project(CoaP)

file(GLOB_RECURSE SRCS *.cpp *.h)

include_directories(
        ../include
        ../src
        ${GTEST_INCLUDE_DIRS}
)

add_executable(testsuite ${SRCS})

target_link_libraries(testsuite coap ${GTEST_BOTH_LIBRARIES})

add_test(NAME testsuite COMMAND testsuite)
