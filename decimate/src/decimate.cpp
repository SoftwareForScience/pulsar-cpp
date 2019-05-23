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
    try {
        po::options_description desc("\ndecimate - reduce time and/or frequency resolution of filterbank data\n\n\
usage: decimate {filename} -{options}\n\noptions");
        desc.add_options()
			("help,h", "produce this help message")
			("filename", po::value<std::string>()->required()->value_name("FILE"), "filterbank data file (def=stdin)")
			(",o", po::value<std::string>()->value_name("FILE"), "filterbank output file (def=stdout)")
			(",c", po::value<int>()->value_name("numchans"), "number of channels to add (def=all)")
            (",t", po::value<int>()->value_name("numsamps"), "number of time samples to add (def=none)")
            (",T", po::value<int>()->value_name("numsamps"), "(alternative to -t) specify number of output timesamples")
            (",n", po::value<int>()->value_name("numbits"), "specify output number of bits (def=input)")
            ("headerless", "do not broadcast resulting header (def=broadcast)");

        po::positional_options_description positionalOptions;
        positionalOptions.add("filename", 1);

        po::variables_map vm;

		try { 
			po::store(po::command_line_parser(argc, argv).options(desc) 
				.positional(positionalOptions).run(), 
				vm);

			if (vm.count("help")) { 
				std::cout << desc << std::endl;
				return SUCCESS; 
			} 

			po::notify(vm);

			} 
		catch (boost::program_options::required_option& ex_required) { 
			std::cerr << ex_required.what() << std::endl;
			return ERROR_IN_COMMAND_LINE; 
		} 
		catch (const po::error &ex) {
			std::cerr << ex.what() << std::endl;
			return ERROR_IN_COMMAND_LINE; 
		}

		int32_t num_chans = 1, num_samps = 1, num_output_samples = 1;		
		bool save_header = true;
		std::string inputFile = vm["filename"].as<std::string>();
		std::string outputFile = "";
		filterbank fb;
		std::vector<std::string> argList(argv, argv + argc);

		if (asteria::file_exists(inputFile)) {
			//Change or remove std::string filename
			fb = filterbank::read_filterbank(inputFile);
			fb.read_data();
		}

		// TODO: Check if file starts with HEADER_START, if so, use the file.
		else {
			//TODO: Add way to read from stdin
			std::cerr << "file: " << inputFile << " does not exist\n";
			return ERROR_UNHANDLED_EXCEPTION;
		}

		//Checking for optional switches
		if (vm.count("-o"))
        {
            fb.filename = vm["-o"].as<std::string>();
        }
		if (vm.count("-c"))
        {
			num_chans = vm["-c"].as<int32_t>();
        }
		if (vm.count("-t"))
        {
            num_samps = vm["-t"].as<int32_t>();
        }
		if (vm.count("-T"))
        {
			num_output_samples = vm["-T"].as<int32_t>();
        }
		if (vm.count("-n"))
        {
            fb.header["nbits"].val.i = vm["-n"].as<int32_t>();;
        }
		//headerless = vm.count("headerless")
		if (vm.count("headerless"))
        {
            save_header = false;
        }

		if (num_chans < 1 || fb.n_channels % num_chans) {
			std::cerr << "File does not contain a multiple of: " << num_chans << " channels.\n";
			exit(-3);
		}
		if (num_samps < 1 || fb.n_samples % num_samps) {
			std::cerr << "File does not contain a multiple of: " << num_samps << " samples.\n";
			exit(-3);
		}

		decimate_channels(fb, num_chans);
		decimate_samples(fb, num_samps);
		fb.save_filterbank(save_header);

	}
	catch (const po::error &ex) {
		std::cerr << ex.what() << '\n';
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