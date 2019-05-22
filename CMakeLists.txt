# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.13)

project ("Asteria")
set(Asteria_VERSION_MAJOR 0)
set(Asteria_VERSION_MINOR 1)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/build)

#include dir
add_subdirectory("libAsteria")
add_subdirectory("decimate")
add_subdirectory("dedisperse")
add_subdirectory("header")

