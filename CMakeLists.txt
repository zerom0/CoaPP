cmake_minimum_required(VERSION 2.8)
project(CoaP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS *.h)

include_directories(include src gtest/googletest/include)

add_library(coap ${SRCS} ${HDRS})
#target_include_directories(coap PUBLIC include)
target_link_libraries(coap pthread)

# Enable the testsuite with
#   git clone https://github.com/google/googletest.git gtest
#IF(EXISTS "gtest/" )
add_subdirectory(gtest)

file(GLOB_RECURSE TSRCS tests/*.cpp)

add_executable(testsuite ${TSRCS} ${HDRS})
#target_include_directories(testsuite PUBLIC include src)
target_link_libraries(testsuite coap gtest_main)

enable_testing()
add_test(NAME testsuite COMMAND testsuite)
#ENDIF()
