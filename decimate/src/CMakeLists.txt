cmake_minimum_required (VERSION 3.13)
set (CMAKE_CXX_STANDARD 11)

project ("decimate")

include_directories("../include")
include_directories("../../libAsteria/filterbank-core/include")
include_directories("../../libAsteria/IO/include")


add_executable(decimate "./decimate.cpp")
target_link_libraries(decimate filterbank-core)
target_link_libraries(decimate asteria)