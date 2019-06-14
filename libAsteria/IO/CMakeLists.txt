cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 11)

project ("asteria")

include_directories("./include")

add_library(asteria "./src/fileutils.cpp")