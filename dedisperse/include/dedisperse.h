#ifndef DEDISPERSE_H
#define DEDISPERSE_H

#include <queue>
#include "filterbank.h"
#include "linspaced.h"


float* dedisperse(filterbank* fb, int highest_x = 0);
std::pair<unsigned int, unsigned int> find_line(filterbank* fb, unsigned int start_sample, double max_delay, float pulsar_intensity);
float find_estimation_intensity(filterbank& fb, int highest_x);
float find_dispersion_measure(filterbank& fb, double max_delay, float pulsar_intensity);

void dedisperse_help();
#endif // !DEDISPERSE_H
