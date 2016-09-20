cmake_minimum_required(VERSION 2.8.7)
project(coap_server)

file(GLOB_RECURSE SRCS *.cpp)

include_directories(../../coap/include)

add_executable(coap_server ${SRCS})
target_link_libraries(coap_server coap)
