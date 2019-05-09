cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 11)

project ("filterbank")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")

find_package(xtensor REQUIRED)
include_directories("../include")

add_library(filterbank "./filterbank.cpp")
target_include_directories(filterbank PUBLIC ${xtensor_INCLUDE_DIRS})
target_link_libraries(filterbank PUBLIC xtensor)
