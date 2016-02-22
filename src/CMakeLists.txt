cmake_minimum_required(VERSION 2.8)
project(CoaP)

file(GLOB_RECURSE SRCS *.cpp)

include_directories(../include)

add_library(coap ${SRCS})
#target_include_directories(coap PUBLIC ../include)
target_link_libraries(coap pthread)
