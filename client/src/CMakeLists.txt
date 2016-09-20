cmake_minimum_required(VERSION 2.8.7)
project(coap_client)

file(GLOB_RECURSE SRCS *.cpp)

include_directories(../../coap/include)

add_executable(coap_client ${SRCS})
target_link_libraries(coap_client coap)
