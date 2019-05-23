#ifndef DECIMATE_H
#define DECIMATE_H

#include <vector>
#include <string>
#include <iostream>
#include "filterbank-core.h"
#include "fileutils.h"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
namespace po = boost::program_options;
namespace {
    const size_t ERROR_IN_COMMAND_LINE = 1;
    const size_t SUCCESS = 0;
    const size_t ERROR_UNHANDLED_EXCEPTION = 2;
}

void decimate_samples(filterbank& fb, uint32_t n_samples_to_combine);
void decimate_channels(filterbank& fb, uint32_t n_channels_to_combine);
void show_usage(std::string name);
#endif // !DECIMATE_H
