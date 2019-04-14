cmake_minimum_required (VERSION 3.13)
set (CMAKE_CXX_STANDARD 11)

project ("asteria")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")

include_directories("../include")

add_library(asteria "./fileutils.cpp")