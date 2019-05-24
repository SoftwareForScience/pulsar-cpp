#include "decimate.h"

int32_t main(int32_t argc, char* argv[]) {

    //BEGIN LEGACY
    //-headerless is not a valid switch, changing it to --headerless.
    int count = 0;
    std::vector<std::string> arguments(argv, argv + argc);
    for (std::string &s : arguments)
    {
        if (!s.compare("-headerless"))
        {
            argv[count] = "--headerless";
        }
        count++;
    }
    //ENDLEGACY

	int32_t num_chans = 1, num_samps = 1, num_output_samples = 1;
	bool save_header = true;
	filterbank fb;
	std::string inputFile = "";
	std::string outputFile = "";

	CommandLineOptions opts;
	CommandLineOptions::statusReturn_e argumentStatus = opts.parse(argc, argv);
	if (argumentStatus == CommandLineOptions::OPTS_SUCCESS) {	
		inputFile = opts.getInputFile();
		
		if (asteria::file_exists(inputFile)) {
			fb = filterbank::read_filterbank(inputFile);
			fb.read_data();
		}
		else {
			//TODO: Add way to read from stdin
			std::cerr << "file: " << inputFile << " does not exist\n";
			return 1;
		}

		if (opts.getHeaderlessFlag()) {
			save_header = false;
		}

		//TODO: Send to stdout
		outputFile = opts.getOutputFile();
		//TODO: Check if folder exists
		if (asteria::file_exists(outputFile)) {
			fb.filename = outputFile;
		} else {
			fb.filename = inputFile;
		}
		
		num_chans = opts.getNumberOfChannels();
		num_samps = opts.getNumberOfSamples();

		num_output_samples = opts.getNumberOfOutputSamples();
		if (num_output_samples > 1) {
			num_samps = fb.n_samples / num_output_samples;
		}

		if (opts.getNumberOfBits()) {
			fb.header["nbits"].val.i = opts.getNumberOfBits();
		}
		
		if (fb.n_channels % num_chans) {
			std::cerr << "File does not contain a multiple of: " << num_chans << " channels.\n";
			exit(-3);
		}
		if (fb.n_samples % num_samps) {
			std::cerr << "File does not contain a multiple of: " << num_samps << " samples.\n";
			exit(-3);
		}

		if (num_chans > 1) {
			decimate_channels(fb, num_chans);
		}
		if (num_samps > 1) {
			decimate_samples(fb, num_samps);
		}
		fb.save_filterbank(save_header);

	}
	else if (argumentStatus == CommandLineOptions::OPTS_HELP) {
		//Help printed
	}
	else {
		std::cerr << "Something went wrong." << std::endl;
	}
}

void decimate_channels(filterbank& fb, uint32_t n_channels_to_combine) {

	uint32_t n_channels_out = fb.n_channels / n_channels_to_combine;
	uint32_t n_values_out = fb.n_ifs * n_channels_out * fb.n_samples;

	std::vector<float> temp(n_values_out);

	for (uint32_t sample = fb.start_sample; sample < fb.end_sample; sample++) {
		for (uint32_t interface = 0; interface < fb.n_ifs; interface++) {
			uint32_t channel = fb.start_channel;
			while (channel < fb.end_channel) {
				float total = 0;

				for (uint32_t j = 0; j < n_channels_to_combine; ++j) {
					int32_t index = (sample * fb.n_ifs * fb.n_channels) + (interface * fb.n_channels) + channel;
					total += fb.data[index];
					channel++;
				}
				float avg = total / n_channels_to_combine;

				uint32_t out_index = (sample * fb.n_ifs * n_channels_out)
					+ (interface * n_channels_out)
					+ ((channel / n_channels_to_combine) - 1);
				temp[out_index] = avg;
			}
		}
	}

	fb.header["nchans"].val.i = n_channels_out;
	fb.n_channels = n_channels_out;
	fb.data = temp;
}

void decimate_samples(filterbank& fb, uint32_t n_samples_to_combine) {
	uint32_t n_samples_out = fb.n_samples / n_samples_to_combine;
	uint32_t n_values_out = fb.n_ifs * fb.n_channels * n_samples_out;

	std::vector<float> temp(n_values_out);

	for (uint32_t channel = fb.start_channel; channel < fb.end_channel; channel++) {
		for (uint32_t interface = 0; interface < fb.n_ifs; interface++) {
			uint32_t sample = fb.start_sample;

			while (sample < fb.end_sample) {
				float total = 0;
				for (uint32_t j = 0; j < n_samples_to_combine; ++j) {
					uint32_t index = (sample * fb.n_ifs * fb.n_channels) + (interface * fb.n_channels) + channel;
					total += fb.data[index];
					sample++;
				}

				float avg = total / n_samples_to_combine;

				int32_t out_index = (((((sample) / n_samples_to_combine) - 1) * fb.n_ifs * fb.n_channels)
					+ (interface * fb.n_channels)
					+ channel);
				temp[out_index] = avg;
			}
		}
	}

	fb.header["nsamples"].val.i = n_samples_out;
	fb.n_samples = n_samples_out;

	// if we decrease the amount of samples, the time between samples increase
	fb.header["tsamp"].val.d = fb.header["tsamp"].val.d * n_samples_to_combine;
	fb.data = temp;
}