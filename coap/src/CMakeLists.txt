cmake_minimum_required(VERSION 2.8.7)
project(CoaP)

file(GLOB_RECURSE SRCS *.cpp)

include_directories(../include)

add_library(coap SHARED ${SRCS})

target_link_libraries(coap pthread)

install(TARGETS coap LIBRARY DESTINATION lib)
