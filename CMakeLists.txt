# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Asteria_Cpp")
set(Asteria_Cpp_VERSION_MAJOR 0)
set(Asteria_Cpp_VERSION_MINOR 1)

#include dir
add_subdirectory("common")
add_subdirectory("filterbank")
add_subdirectory("decimate")
#add_subdirectory("header")



