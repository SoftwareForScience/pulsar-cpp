#include "decimate.h"

int main(int argc, char* argv[]) {
	filterbank fb;
	CommandLineOptions opts;
	legacy_arguments(argc, argv, opts);

	CommandLineOptions::statusReturn_e argumentStatus = opts.parse(argc, argv);
	if (argumentStatus == CommandLineOptions::OPTS_SUCCESS) {
		fb = filterbank::read_filterbank(opts.getInputFile());
		fb.read_data();
		if ((opts.getOutputFile()).length() != 0) {
			fb.filename = opts.getOutputFile();
		} else {
			fb.filename = opts.getInputFile();
		}

		// if (opts.getNumberOfBits()) {
		// 	fb.header["nbits"].val.i = opts.getNumberOfBits();
		// }

		if (opts.getNumberOfOutputSamples()) {
			decimate_samples(fb, (fb.n_samples / opts.getNumberOfOutputSamples()));
		}
		else {
			decimate_samples(fb, opts.getNumberOfSamples());
		}
		decimate_channels(fb, opts.getNumberOfChannels());

		fb.save_filterbank(!opts.getHeaderlessFlag());
	}
	else if (argumentStatus == CommandLineOptions::OPTS_HELP) {
		//Help printed
		//Do something else?
	}
	else {
		std::cerr << "Something went wrong." << std::endl;
		//do sOmEtHiNg
	}
}

void decimate_channels(filterbank& fb, uint32_t n_channels_to_combine) {
	if (n_channels_to_combine < 1 || fb.n_channels % n_channels_to_combine) {
		std::cerr << "File does not contain a multiple of: " << n_channels_to_combine << " channels.\n";
		exit(-3);
	}

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
	fb.end_channel = n_channels_out - fb.start_channel;
	fb.data = temp;
}

void decimate_samples(filterbank& fb, uint32_t n_samples_to_combine) {
	if (n_samples_to_combine < 1 || fb.n_samples % n_samples_to_combine) {
		std::cerr << "File does not contain a multiple of: " << n_samples_to_combine << " samples.\n";
		exit(-3);
	}

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
	fb.end_sample = n_samples_out - fb.start_sample;

	// if we decrease the amount of samples, the time between samples increase
	fb.header["tsamp"].val.d = fb.header["tsamp"].val.d * n_samples_to_combine;
	fb.data = temp;
}

void legacy_arguments(int argc, char* argv[], CommandLineOptions& opts) {
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
}