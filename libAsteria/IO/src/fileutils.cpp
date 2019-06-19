#include "fileutils.h"

/**
 * @brief Checks whether a file exists
 * 
 * param[in] fileName the filename to check
 */
bool asteria::file_exists(std::string fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}