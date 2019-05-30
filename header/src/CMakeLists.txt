cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 14)

project ("header")

include_directories("../include")
include_directories("../../libAsteria/filterbank-core/include")

set(Boost_NO_BOOST_CMAKE TRUE)
find_package(Boost 1.70.0 REQUIRED COMPONENTS date_time)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

if(Boost_FOUND)
    add_executable(header "./header.cpp")
    target_link_libraries(header filterbank-core)
    target_link_libraries(header asteria)
    target_link_libraries(header ${Boost_LIBRARIES})
endif()