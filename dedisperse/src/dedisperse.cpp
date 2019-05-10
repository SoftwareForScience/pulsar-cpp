#include "dedisperse.h"

int main(int argc, char* argv[]) {
	//std::vector<std::string> argList(argv, argv + argc);
	//if (argList.size() == 1)
	//	std::cout << "Please supply a filterbank file \n";

	//std::string filename = argList[1];
	std::string filename = argv[1];

	auto fb = filterbank::read_filterbank(filename);
	float avg_intensity = find_estimation_intensity(fb,10);
}


float* dedisperse(filterbank& fb, int highest_x)
{
	return nullptr;
}

bool find_line(filterbank* fb, unsigned int start_sample, double max_delay, float pulsar_intensity)
{
	
	unsigned int previous_index = start_sample;
	bool found_line = false;

	return found_line;
}


float find_dispersion_measure(filterbank& fb, double max_delay, float pulsar_intensity)
{
	//int len = fb.header["nsamples"].val.i;

	//int start_sample_index;

	//for (int i = 0; i < fb.n_samples_c; ++i) {
	//	float sample = ((float*) & (fb.data))[i];

	//	if (sample > pulsar_intensity) {
	//		start_sample_index = i;

	//		// Attemt to find a line, 

	//		//line_coordinates = find_line(samples, len, max_delay, pulsar_intensity)
	//	}
	//}

	return 0.0f;
}

float find_estimation_intensity(filterbank& fb, int highest_x)
{
	//int len = fb.n_samples_c;

	//float sum_intensities = 0.0;
	//for (int channel = 0; channel < fb.n_channels; channel++) {
	//	
	//	//sum the highest n values per channel;
	//	std::priority_queue<std::pair<float, int>> q;
	//	for (int sample = 0; sample < len; ++sample) {
	//		q.push(std::pair<float, int>(((float*) & (fb.data))[channel], sample));
	//	}

	//	for (int i = 0; i < highest_x; ++i) {
	//		float val = q.top().first;
	//		sum_intensities += val;
	//		q.pop();
	//	}
	//}

	//float average_intensity = (sum_intensities / (len * highest_x));
	//return average_intensity;
	return 0.0;
}
