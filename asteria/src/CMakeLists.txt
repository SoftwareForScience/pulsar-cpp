cmake_minimum_required (VERSION 3.13)
set (CMAKE_CXX_STANDARD 11)

project ("asteria")

include_directories("../include")

add_library(asteria "./fileutils.cpp")