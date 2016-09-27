cmake_minimum_required(VERSION 2.8.7)
project(CoaP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(coap/src)

add_subdirectory(client/src)
add_subdirectory(server/src)

enable_testing()
add_subdirectory(coap/tests)
