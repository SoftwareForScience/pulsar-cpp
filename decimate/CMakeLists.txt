cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 11)

project ("decimate")

include_directories("./include")
include_directories("../libAsteria/filterbankCore/include")
include_directories("../libAsteria/IO/include")

set(Boost_NO_BOOST_CMAKE TRUE)
find_package(Boost 1.70.0 REQUIRED COMPONENTS program_options)
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)

if(Boost_FOUND)
    add_executable(decimate "./src/decimate.cpp" "./src/CommandLineOptions.cpp")
    target_link_libraries(decimate filterbankCore)
    target_link_libraries(decimate asteria)
    target_link_libraries(decimate ${Boost_LIBRARIES})
endif()