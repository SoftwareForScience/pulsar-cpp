#ifndef DECIMATE_H
#define DECIMATE_H

#include <vector>
#include <string>
#include <iostream>
#include "filterbankCore.hpp"
#include "fileutils.h"
#include "CommandLineOptions.hpp"

void decimate_samples(filterbank& fb, uint32_t n_samples_to_combine);
void decimate_channels(filterbank& fb, uint32_t n_channels_to_combine);
void legacy_arguments(int argc, char* argv[], CommandLineOptions& opts);
#endif // !DECIMATE_H
