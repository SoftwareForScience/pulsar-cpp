#include "dedisperse.h"

int32_t main(int32_t argc, char* argv[]) {
	std::vector<std::string> argList(argv, argv + argc);
	if (argList.size() < 1)
		dedisperse_help();

	std::string filename = argv[1];

	auto fb = filterbank::read_filterbank(filename);
	fb.read_data();

	dedisperse(fb, (double)10, (float)10, 10);

}


void dedisperse(filterbank& fb, double max_delay, float dispersion_measure, uint32_t highest_x)
{
	if (!dispersion_measure) {
		float pulsar_intensity = find_estimation_intensity(fb, 10);
		dispersion_measure = find_dispersion_measure(fb, pulsar_intensity, max_delay);
	}

	std::vector<double> delays_per_sample = linspace(dispersion_measure, (float)0, (int)fb.n_samples);

	float* temp = new float[fb.n_samples];
	for (uint32_t channel = 0; channel < fb.n_channels; channel++) {
		// fill temp array with the for a single channel
		for (uint32_t sample = 0; sample < fb.n_samples; sample++)
		{
			uint32_t index = (sample * fb.n_ifs * fb.n_channels) + channel;
			temp[sample] = fb.data[index];
		}

		// rotate over the time axis
		//std::rotate(temp[0], temp[0] + static_cast<float>(delays_per_sample[channel]), temp[fb.n_samples]);

		//write back to array
		for (uint32_t sample = 0; sample < fb.n_samples; sample++)
		{
			int32_t index = (sample * fb.n_ifs * fb.n_channels) + channel;
			fb.data[index] = temp[sample];
		}
	}
}

std::pair<uint32_t, uint32_t> find_line(filterbank& fb, uint32_t start_sample, double max_delay, float pulsar_intensity)
{
	uint32_t previous_index = start_sample;
	bool found_line = false;

	// Loop through the channels
	for (uint32_t channel = 0; channel < fb.n_channels; ++channel) {
		//Loop through previous samples
		for (uint32_t sample = start_sample; sample < fb.n_samples; ++sample) {

		}
	}

	return std::pair<uint32_t, uint32_t>(0, 0);
}


float find_dispersion_measure(filterbank& fb, float pulsar_intensity, double max_delay)
{
	uint32_t start_sample_index = 0;
	std::pair<uint32_t, uint32_t> line_coordinates;

	// loop through the samples to find a pulsar intensity to start calcultating from
	for (uint32_t sample = 0; sample < fb.n_samples; ++sample) {
		int32_t sample_index = (sample * fb.n_ifs * fb.n_channels);
		for (uint32_t channel = 0; channel < fb.n_channels; ++channel) {
			//if the sample meets the minimum intensity, attempt to find a line continueing from the intensity
			if (fb.data[((uint64_t)sample_index) + channel] > pulsar_intensity) {
				start_sample_index = sample;

				//attempt to find a line, line_coordinates contains the first and last index of the pulsar
				line_coordinates = find_line(fb, start_sample_index, max_delay, pulsar_intensity);
			}
		}
	}

	return 0.0f;
}

float find_estimation_intensity(filterbank& fb, uint32_t highest_x)
{
	float sum_intensities = 0.0;

	//sum the highest n values per sample;
	for (uint32_t sample = 0; sample < fb.n_samples; ++sample) {
		uint32_t sample_index = (sample * fb.n_ifs * fb.n_channels);

		std::priority_queue<float> q;
		for (uint32_t channel = 0; channel < fb.n_channels; ++channel) {
			q.push(fb.data[((uint64_t)sample_index) + channel]);
		}

		for (uint32_t i = 0; i < highest_x; ++i) {
			float val = q.top();
			sum_intensities += val;
			q.pop();
		}
	}


	float average_intensity = (sum_intensities / (fb.n_samples * highest_x));
	return average_intensity;
}


void dedisperse_help() /*includefile*/
{
	std::cout << std::endl;
	std::cout << ("dedisperse  - form time series from filterbank data or profile from folded data") << std::endl << std::endl;
	std::cout << ("usage: dedisperse {filename} -{options}") << std::endl << std::endl;
	std::cout << ("options:") << std::endl << std::endl;
	std::cout << ("   filename - full name of the raw data file to be read (def=stdin)") << std::endl;
	std::cout << ("-d dm2ddisp - set DM value to dedisperse at (def=0.0)") << std::endl;
	std::cout << ("-b numbands - set output number of sub-bands (def=1)") << std::endl;
	std::cout << ("-B num_bits - set output number of bits (def=32)") << std::endl;
	std::cout << ("-o filename - output file name (def=stdout)") << std::endl;
	std::cout << ("-c minvalue - clip samples > minvalue*rms (def=noclip)") << std::endl;
	std::cout << ("-f reffreq  - dedisperse relative to refrf MHz (def=topofsubband)") << std::endl;
	std::cout << ("-F newfreq  - correct header value of centre frequency to newfreq MHz (def=header value)") << std::endl;
	std::cout << ("-n num_bins - set number of bins if input is profile (def=input)") << std::endl;
	std::cout << ("-i filename - read list of channels to ignore from a file (def=none)") << std::endl;
	std::cout << ("-p np1 np2  - add profile numbers np1 thru np2 if multiple WAPP dumps (def=all)") << std::endl;
	std::cout << ("-j Jyfactor - multiply dedispersed data by Jyfactor to convert to Jy") << std::endl;
	std::cout << ("-J Jyf1 Jyf2 - multiply dedispersed data by Jyf1 and Jyf2 to convert to Jy (use only for two-polarization data)") << std::endl;
	std::cout << ("-wappinvert - invert WAPP channel order (when using LSB data) (def=USB)") << std::endl;
	std::cout << ("-wappoffset - assume wapp fsky between two middle channels (for pre-52900 data ONLY)") << std::endl;
	std::cout << ("-rmean      - subtract the mean of channels from each sample before dedispersion (def=no)") << std::endl;
	std::cout << ("-swapout    - perform byte swapping on output data (def=native)") << std::endl;
	std::cout << ("-nobaseline - don't subtract baseline from the data (def=subtract)") << std::endl;
	std::cout << ("-sumifs     - sum 2 IFs when creating the final profile (def=don't)") << std::endl;
	std::cout << ("-headerless - write out data without any header info") << std::endl;
	std::cout << ("-epn        - write profiles in EPN format (def=ASCII)") << std::endl;
	std::cout << ("-asciipol   - write profiles in ASCII format for polarization package") << std::endl;
	std::cout << ("-stream     - write profiles as ASCII streams with START/STOP boundaries") << std::endl << std::endl;
}