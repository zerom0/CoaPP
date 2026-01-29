cmake_minimum_required(VERSION 3.5)
project(CoaP)

set(SRCS
        fuzzer.cpp
        ../src/Message.cpp
        ../src/Path.cpp)

include_directories(
        ../include
        ../src
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

add_executable(fuzzer ${SRCS})

target_link_libraries(fuzzer)