cmake_minimum_required (VERSION 3.13)
set (CMAKE_CXX_STANDARD 11)

project ("header")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")

include_directories("../include")
include_directories("../../filterbank/include")

add_executable(header "./header.cpp")
target_link_libraries(header filterbank)