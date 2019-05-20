#ifndef DECIMATE_H
#define DECIMATE_H

#include <vector>
#include <string>
#include <iostream>
#include "filterbank-core.h"
#include "fileutils.h"

void decimate_samples(filterbank& fb, uint32_t n_samples_to_combine);
void decimate_channels(filterbank& fb, uint32_t n_channels_to_combine);
void show_usage(std::string name);
#endif // !DECIMATE_H
