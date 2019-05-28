#include "decimate.h"

int32_t main(int32_t argc, char* argv[]) {

	std::vector<std::string> argList(argv, argv + argc);
	if (argList.size() < 2) {
		show_usage(argList[0]);
		exit(-1);
	}

	int32_t num_chans = 1, num_samps = 1, num_output_samples = 1;
	bool save_header = true;
	std::string outputFile = "";
	filterbank fb;

	if (asteria::file_exists(argList[1])) {
		std::string filename = argList[1];
		fb = filterbank::read_filterbank(filename);
		fb.read_data();
	}

	// TODO: Check if file starts with HEADER_START, if so, use the file.
	else {
		show_usage("Decimate");
		std::cerr << "file: " << argList[1] << " does not exist\n";
		exit(-1);
	}
	//TODO: Better error handling
	for (int32_t i = 2; i < argList.size(); i++) {
		if (!argList[i].compare("-c")) {
			num_chans = atoi(argv[++i]);
			if (num_chans <= 0) {
				show_usage("Decimate");
				std::cerr << "Number of channels must be greater than 0\n";
				exit(-1);
			}
		}
		else if (!argList[i].compare("-o")) {
			fb.filename = argList[++i];
		}
		else if (!argList[i].compare("-t")) {
			num_samps = atoi(argv[++i]);
			if (num_samps <= 0) {
				show_usage("Decimate");
				std::cerr << "Number of samples must be greater than 0\n";
				exit(-1);
			}
		}
		else if (!argList[i].compare("-T")) {
			num_output_samples = atoi(argv[++i]);
			if (num_output_samples <= 0) {
				show_usage("Decimate");
				std::cerr << "Number of output timesamples must be greater than 0\n";
				exit(-1);
			}
			num_samps = fb.n_samples / num_output_samples;
		}
		else if (!argList[i].compare("-n")) {
			fb.header["nbits"].val.i = atoi(argv[++i]);
		}
		else if (!argList[i].compare("-headerless")) {
			save_header = false;
		}
		else {
			show_usage("Decimate");
			std::cerr << "unknown argument " << argv[i] << " passed to decimate\n";
			exit(-2);
		}
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
	fb.end_channel = n_channels_out - fb.start_channel;
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
	fb.end_sample = n_samples_out - fb.start_sample;

	// if we decrease the amount of samples, the time between samples increase
	fb.header["tsamp"].val.d = fb.header["tsamp"].val.d * n_samples_to_combine;
	fb.data = temp;
}

void show_usage(std::string name) {
	std::cerr
		<< name << " - reduce time and/or frequency resolution of filterbank-core data\n" << std::endl
		<< "usage: " << name << "{filename} -{options}\n" << std::endl
		<< "options: \n" << std::endl
		<< "   filename - filterbank-core data file (def=stdin)" << std::endl
		<< "-o filename - output filterbank-core data file" << std::endl
		<< "-c numchans - number of channels to add (def=all)" << std::endl
		<< "-t numsamps - number of time samples to add (def=none)" << std::endl
		<< "-T numsamps - (alternative to -t) specify numberof output timesamples" << std::endl
		<< "-n numbits  - specify output numberof bits(def=input)" << std::endl
		<< "-headerless - do not broadcast resulting header(def=broadcast)" << std::endl;
}