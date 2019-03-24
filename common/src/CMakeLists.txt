#Project name
project ("libAsteriaCommon")

add_library(common "fileutils.cpp")

#include dir
target_include_directories(common PUBLIC "../include")

export(TARGETS common FILE AsteriaCommon.cmake)