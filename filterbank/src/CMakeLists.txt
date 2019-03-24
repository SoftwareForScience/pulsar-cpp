#Project name
project ("filterbank")
							 
include_directories("../include")

add_library(filterbank "filterbankFile.cpp")
target_link_libraries(filterbank common)

#add_library(filterbank "filterbankFile.cpp" "filterbankHeader.cpp" "filterbankOutput.cpp")
					 
#include dir
#target_include_directories(filterbank PUBLIC "../include")

#export(TARGETS filterbank FILE Filterbank.cmake)
