#Project name
project ("decimate")

#include dir
include_directories("../include")
add_executable(decimate "decimate.cpp")							
target_link_libraries(decimate common)