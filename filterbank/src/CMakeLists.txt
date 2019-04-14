cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 11)

project ("filterbank")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
include_directories ($ENV{EIGEN3_INCLUDE_DIR})
include_directories("../include")

add_library(filterbank "./filterbank.cpp")
