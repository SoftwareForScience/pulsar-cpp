#include "filterbankCore.hpp"

filterbank filterbank::read_stdio(std::string input) {
	auto fb = filterbank();

	if (input.compare("")) {
		std::cerr << "No valid input given";
		return fb;
	}

	fb.read_header_stdio(input);
	return fb;
}

bool filterbank::read_header_stdio(std::string input) {
	unsigned int index = 0;
	// read key length from string
	uint32_t keylen = std::stoul(input.substr(index, sizeof(uint32_t)), nullptr, 0);
	// move the "file pointer"
	index += sizeof(keylen);
	// Read the string
	const std::string initial = input.substr(index, keylen);
	// move our "file pointer"
	index += keylen;

	// Compare returns  > 0 if initial is diffent, therefore it doesnt 
	// start with header_start.
	if (initial.compare("HEADER_START")) {
		std::cerr << "Error, File is not a valid filterbank file";
		return false;
	}

	while (true) {
		//Get size of token
		keylen = std::stoul(input.substr(index, sizeof(uint32_t)), nullptr, 0);
		index += sizeof(keylen);

		std::string token = input.substr(index, keylen);
		index += keylen;
			
		if (!token.compare("HEADER_END")) {
			// get size of the header by getting the current position;
			header_size = index;
			break;
		}

		switch (header[token].type) {
		case INT: {
			header[token].val.i = std::stol(input.substr(index, sizeof(uint32_t)), nullptr,0);
			index += sizeof(uint32_t);
			break;
		}
		case DOUBLE: {
			header[token].val.d = std::stod(input.substr(index, sizeof(double)), nullptr);
			index += sizeof(double);
			break;
		}
		case STRING: {
			// Get size of string
			uint32_t valLen = std::stoul(input.substr(index, sizeof(uint32_t)), nullptr, 0);
			index += sizeof(valLen);

			// Get value of string
			auto value = input.substr(index, valLen);
			strncpy(header[token].val.s, value.c_str(), valLen);
			break;
		}
		};

	}
	file_size = sizeof(input);
	data_size = file_size - header_size;

	n_channels = header["nchans"].val.i;
	n_ifs = header["nifs"].val.i;
	center_freq = (header["fch1"].val.d + n_channels * header["foff"].val.d / 2.0);
	n_bytes = header["nbits"].val.i / 8;

	telescope = telescope_ids[header["telescope_id"].val.i];
	backend = machine_ids[header["machine_id"].val.i];

	// if nsamples isn't set, get it from the data size
	if (!header["nsamples"].val.i) {
		header["nsamples"].val.i = data_size / (n_bytes * n_channels * n_ifs);
	}

	n_samples = header["nsamples"].val.i;
	n_values = n_ifs * n_channels * n_samples;

	return true;
}


bool filterbank::write_stdio(bool headerless) {
	if (!headerless) {
		std::cout << "HEADER_START";
		for (auto param : header)
		{
			//Skip unused headers
			if (param.second.val.d == 0.0) {
				continue;
			}
			switch (param.second.type) {
			case INT: {
				std::cout << param.first << param.second.val.i;
				break;
			}
			case DOUBLE: {
				std::cout << param.first << param.second.val.d;
				break;
			}
			case STRING: {
				std::cout << param.first << param.second.val.s;
				break;
			}
			}
		}
		std::cout << "HEADER_END";
	}

	for (uint32_t sample = 0; sample < n_samples; ++sample) {
		for (uint32_t interface = 0; interface < n_ifs; ++interface) {
			// Get the index for the interface
			int32_t index = (sample * n_ifs * n_channels) + (interface * n_channels);
			switch (n_bytes) {
			case 1: {
				std::vector<uint8_t>cwbuf(n_channels);
				for (uint32_t channel = start_channel; channel < end_channel; channel++) {
					cwbuf[channel - start_channel] = (uint8_t)data[((uint64_t)index) + channel];
				}
				
				fwrite(&cwbuf[0], sizeof(uint8_t), cwbuf.size(), stdout);
				break;
			}
			case 2: {
				std::vector<uint16_t>swbuf(n_channels);
				for (uint32_t channel = start_channel; channel < end_channel; channel++) {
					swbuf[channel - start_channel] = (uint16_t)data[((uint64_t)index) + channel];
				}

				fwrite(&swbuf[0], sizeof(uint16_t), swbuf.size(), stdout);

				break;
			}
			case 4: {
				fwrite(&data[index], sizeof(uint32_t), data.size(), stdout);
				break;
			}
			}
		}
	}
}
