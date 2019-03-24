#pragma once
#include <stdio.h>

namespace Asteria {
	FILE* file_open(char* filename, char* descriptor);
	int file_exists(char* filename);
}