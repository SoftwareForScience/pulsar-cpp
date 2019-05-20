cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 11)

project ("header")

include_directories("../include")
include_directories("../../libAsteria/filterbank-core/include")

add_executable(header "./header.cpp")
target_link_libraries(header filterbank-core)