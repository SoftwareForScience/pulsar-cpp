cmake_minimum_required (VERSION 3.13)
set (CMAKE_CXX_STANDARD 11)

project ("decimate")

include_directories("../include")
include_directories("../../filterbank/include")
include_directories("../../asteria/include")


add_executable(decimate "./decimate.cpp")
target_link_libraries(decimate filterbank)
target_link_libraries(decimate asteria)