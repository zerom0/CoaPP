cmake_minimum_required(VERSION 2.8)
project(CoaP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HDRS *.h)

include_directories("include")

add_library(coap ${SRCS} ${HDRS})
target_link_libraries(coap pthread)

#file(GLOB_RECURSE TSRCS tests/*.cpp)

#add_executable(testsuite ${TSRCS} ${HDRS})
#target_link_libraries(testsuite coap)
