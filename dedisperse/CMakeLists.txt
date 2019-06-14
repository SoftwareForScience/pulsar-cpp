cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 11)

project ("dedisperse")

include_directories("./include")
include_directories("../libAsteria/filterbankCore/include")

add_executable(dedisperse "./src/dedisperse.cpp")

target_link_libraries(dedisperse filterbankCore)