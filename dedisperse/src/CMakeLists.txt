cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 11)

project ("dedisperse")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")

include_directories("../include")
include_directories("../../filterbank/include")
include_directories ($ENV{EIGEN3_INCLUDE_DIR})

add_executable(dedisperse "./dedisperse.cpp")

target_link_libraries(dedisperse filterbank)