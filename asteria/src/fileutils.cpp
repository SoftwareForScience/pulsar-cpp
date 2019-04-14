#include "fileutils.h"

bool asteria::file_exists(std::string fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}