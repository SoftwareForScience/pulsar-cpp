#ifndef DECIMATE_H
#define DECIMATE_H

#include <vector>
#include <string>
#include <iostream>
#include "filterbank.h"
#include "fileutils.h"

void decimate(filterbank& fb, std::string outputFileName, unsigned int n_samples_to_combine);

#endif // !DECIMATE_H
