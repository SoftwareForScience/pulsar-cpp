#Project name
project ("filterbank")

add_library(filterbank "filterbankFile.cpp" )
					 
#include dir
target_include_directories(filterbank PUBLIC "../include")

export(TARGETS filterbank FILE AsteriaFilterbank.cmake)
