cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 11)

project ("filterbank-core")

include_directories("./include")

add_library(filterbank-core "./src/filterbank-core.cpp")