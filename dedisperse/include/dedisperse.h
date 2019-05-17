#ifndef DEDISPERSE_H
#define DEDISPERSE_H

#include <queue>
#include "filterbank.h"
#include "linspaced.h"


void dedisperse(filterbank& fb, double max_delay, float dispersion_measure, uint32_t highest_x);
std::pair<uint32_t, uint32_t> find_line(filterbank* fb, uint32_t start_sample, double max_delay, float pulsar_intensity);
float find_estimation_intensity(filterbank& fb, uint32_t highest_x);
float find_dispersion_measure(filterbank& fb, float pulsar_intensity, double max_delay);

void dedisperse_help();
#endif // !DEDISPERSE_H
