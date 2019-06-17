#include "filterbankCore.hpp"

filterbank filterbank::read_stdio(std::string input) {
	auto fb = filterbank();

	if (input.compare("")) {
		std::cerr << "No valid input given";
		return fb;
	}

	fb.read_header_stdio(input);
	fb.read_data_stdio(input);
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

	center_freq = (header["fch1"].val.d + header["nchans"].val.i * header["foff"].val.d / 2.0);
	n_bytes = header["nbits"].val.i / 8;

	telescope = telescope_ids[header["telescope_id"].val.i];
	backend = machine_ids[header["machine_id"].val.i];

	// if nsamples isn't set, get it from the data size
	if (!header["nsamples"].val.i) {
		header["nsamples"].val.i = data_size / (n_bytes * header["nchans"].val.i * header["nifs"].val.i);
	}

	n_values = header["nifs"].val.i * header["nchans"].val.i * header["nsamples"].val.i;

	return true;
}

bool filterbank::read_data_stdio(std::string input) {
	size_t samples_read = 0;
	uint32_t sample = 0;

	// Allocate a block of data
	data = std::vector<float>(n_values);


	auto header_end = header_size;
	auto nread = header["nsamples"].val.i * header["nifs"].val.i * header["nchans"].val.i;

	unsigned int data_size = 0;

	/* decide how to read the data based on the number of bits per sample */
	switch (header["nbits"].val.i) {
	case 8: /* read n bytes into character block containing n 1-byte numbers */
		data_size = sizeof(uint8_t);
		for (unsigned int i = 0; i < nread; i++)
		{
			data[i] = input.substr(header_end + (i * data_size) , data_size)[0];
		}
		break;

	case 16: /* read 2*n bytes into short block containing n 2-byte numbers */
		data_size = sizeof(uint16_t);

		for (unsigned int i = 0; i < nread; i++) {
			// Converts the value (to an unsigned long (uint32_t)
			data[i] = std::stoul(input.substr(header_end + (i * data_size), data_size), nullptr, 0);
		}

		break;
	case 32:
		data_size = sizeof(float);
		for (unsigned int i = 0; i < nread; i++){
			data[i] = std::stof(input.substr(header_end + (i * data_size), data_size), nullptr);
		}

		break;
	}

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

	for (uint32_t sample = 0; sample < header["nsamples"].val.i; ++sample) {
		for (uint32_t interface = 0; interface < header["nifs"].val.i; ++interface) {
			// Get the index for the interface
			int32_t index = (sample * header["nifs"].val.i * header["nchans"].val.i) + (interface * header["nchans"].val.i);
			switch (n_bytes) {
			case 1: {
				std::vector<uint8_t>cwbuf(header["nchans"].val.i);
				for (uint32_t channel = 0; channel < header["nchans"].val.i; channel++) {
					cwbuf[channel] = (uint8_t)data[((uint64_t)index) + channel];
				}
				
				fwrite(&cwbuf[0], sizeof(uint8_t), cwbuf.size(), stdout);
				break;
			}
			case 2: {
				std::vector<uint16_t>swbuf(header["nchans"].val.i);
				for (uint32_t channel = 0; channel < header["nchans"].val.i; channel++) {
					swbuf[channel] = (uint16_t)data[((uint64_t)index) + channel];
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
