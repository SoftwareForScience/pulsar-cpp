cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 11)

project ("filterbank")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")


include_directories("../include")

add_executable(filterbank "./read_header.cpp" "./header.cpp")

