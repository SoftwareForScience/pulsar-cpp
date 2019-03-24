#include "fileutils.h"

FILE* Asteria::file_open(char* filename, char* descriptor)
{
	FILE* fptr;

	if ((fptr = fopen(filename, descriptor)) == NULL) {
		fprintf(stderr, "Error in opening file: %s\n", filename);
		return nullptr;
	}
	return fptr;
}

int Asteria::file_exists(char* filename)
{
	if ((fopen(filename, "rb")) == NULL) {
		return(0);
	}
	else {
		return(1);
	}
}