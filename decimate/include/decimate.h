#ifndef DECIMATE_H
#define DECIMATE_H

#include <vector>
#include <string>
#include <iostream>
#include "filterbank-core.h"
#include "fileutils.h"
#include "CommandLineOptions.hpp"

void decimate_samples(filterbank& fb, uint32_t n_samples_to_combine);
void decimate_channels(filterbank& fb, uint32_t n_channels_to_combine);
void handle_arguments(int argc, char* argv[], CommandLineOptions& opts);
void set_input_output(filterbank& fb, CommandLineOptions& opts);
#endif // !DECIMATE_H
