#include "filterbankCore.hpp"

template <typename T>
/**
 * @brief Gets a value from string object
 * 
 * @param buffer the string
 * @return T the value parsed
 */
T val_from_string(std::string buffer){
	T out_value =0;
	memcpy(&out_value, buffer.c_str(), sizeof(T));
	return out_value;
}

/**
 * @brief Reads a filterbank file from stdio
 * 
 * @return filterbank The filterbank file to return
 */
filterbank filterbank::read_stdio() {
	auto fb = filterbank();

	char c;
	std::string input = "";
	while (std::cin.good())
	{
		c = std::cin.get();
		input += c;
	}

	if (!input.compare("")) {
		std::cerr << "No valid input given";
		return fb;
	}

	fb.read_header_stdio(input);
	fb.read_data_stdio(input);
	return fb;
}

/**
 * @brief Reads the filterbank header from stdio
 * 
 * @param input the input string
 * @return true on success
 * @return false if the filterbank header is invalid
 */
bool filterbank::read_header_stdio(std::string input) {
	unsigned int index = 0;
	unsigned int total_data_size = 0;
	// read key length from string
	uint32_t keylen = val_from_string<uint32_t>(input.substr(index, sizeof(uint32_t)));

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
		uint32_t keylen = val_from_string<uint32_t>(input.substr(index, sizeof(uint32_t)));
		index += sizeof(keylen);
		std::string token = input.substr(index, keylen);
		index += keylen;
		if (!token.compare("HEADER_END")) {
			// get size of the header by getting the current position;
			header_size = index;
			break;
		}

		unsigned int data_size = 0;
		switch (header[token].type) {
			case INT: {
				data_size = sizeof(uint32_t);
				header[token].val.i = val_from_string<uint32_t>(input.substr(index, data_size));
				index += data_size;
				break;
			}
			case DOUBLE: {
				data_size = sizeof(double);
				header[token].val.d = val_from_string<uint32_t>(input.substr(index, data_size));
				index += data_size;
				break;
			}
			case STRING: {
				data_size = sizeof(uint32_t);
				// Get size of string
				uint32_t valLen = val_from_string<uint32_t>(input.substr(index, data_size));
				index += data_size;
				// Get value of string
				auto value = input.substr(index, valLen);
				strncpy(header[token].val.s, value.c_str(), valLen);
				index += valLen;
				break;
			}
		}
	}
	file_size = input.size();
	total_data_size = file_size - header_size;

	center_freq = (header["fch1"].val.d + header["nchans"].val.i * header["foff"].val.d / 2.0);
	n_bytes = header["nbits"].val.i / 8;

	telescope = telescope_ids[header["telescope_id"].val.i];
	backend = machine_ids[header["machine_id"].val.i];

	// if nsamples isn't set, get it from the data size
	if (!header["nsamples"].val.i) {
		header["nsamples"].val.i = total_data_size / (n_bytes * header["nchans"].val.i * header["nifs"].val.i);
	}

	n_values = header["nifs"].val.i * header["nchans"].val.i * header["nsamples"].val.i;

	return true;
}

bool filterbank::read_data_stdio(std::string input) {
	size_t values_read = 0;
	uint32_t sample = 0;

	// Allocate a block of data
	data = std::vector<float>(n_values);

	std::string buffer;
	unsigned int data_size = 0;

	/* decide how to read the data based on the number of bits per sample */
	switch (header["nbits"].val.i) {
		case 8: { /* read n bytes into character block containing n 1-byte numbers */
			data_size = sizeof(uint8_t);
			for (unsigned int i = 0; i < n_values; i++) {
				buffer = input.substr(header_size + (i * data_size) , data_size);
				data[i] = val_from_string<uint32_t>(buffer);
			}
			break;
		}
		case 16: { /* read 2*n bytes into short block containing n 2-byte numbers */
			data_size = sizeof(uint16_t);
			for (unsigned int i = 0; i < n_values; i++) {
				// Converts the value (to an unsigned long (uint32_t)
				buffer = input.substr(header_size + (i * data_size) , data_size);
				data[i] = val_from_string<uint16_t>(buffer);
			}
			break;
		}
		case 32: {
			data_size = sizeof(float);
			for (unsigned int i = 0; i < n_values; i++){
				buffer = input.substr(header_size + (i * data_size) , data_size);
				val_from_string<float>(buffer);		
			}
			break;
		}
	}
	if (values_read != n_values) {
		return false;
	}
	return true;
}

/**
 * @brief Writes the data from the current filterbank object to a stdio
 * 
 * @param headerless whether or not to write the header
 * @return true on success
 * @return false on failure
 */
bool filterbank::write_stdio(bool headerless) {
	if (!headerless) {
		write_string(stdout, "HEADER_START");
		for (auto param : header) {
			//Skip unused headers
			if (param.second.val.d == 0.0) {
				continue;
			}
			switch (param.second.type) {
				case INT: {
					write_value(stdout, param.first, param.second.val.i);
					break;
				}
				case DOUBLE: {
					write_value(stdout, param.first, param.second.val.d);
					break;
				}
				case STRING: {
					write_string(stdout, param.first);
					write_string(stdout, param.second.val.s);
					break;
				}
			}
		}
		write_string(stdout, "HEADER_END");
	}

	for (uint32_t sample = 0; sample < header["nsamples"].val.i; ++sample) {
		for (uint32_t interface = 0; interface < header["nifs"].val.i; ++interface) {
			// Get the index for the interface
			int32_t index = (sample * header["nifs"].val.i * header["nchans"].val.i) + (interface * header["nchans"].val.i);
			switch (header["nbits"].val.i) {
				case 8: {
					std::vector<uint8_t>cwbuf(header["nchans"].val.i);
					for (uint32_t channel = 0; channel < header["nchans"].val.i; channel++) {
						cwbuf[channel] = (uint8_t)data[((uint64_t)index) + channel];
					}
					fwrite(&cwbuf[0], sizeof(uint8_t), cwbuf.size(), stdout);
					break;
				}
				case 16: {
					std::vector<uint16_t>swbuf(header["nchans"].val.i);
					for (uint32_t channel = 0; channel < header["nchans"].val.i; channel++) {
						swbuf[channel] = (uint16_t)data[((uint64_t)index) + channel];
					}
					fwrite(&swbuf[0], sizeof(uint16_t), swbuf.size(), stdout);
					break;
				}
				case 32: {
					fwrite(&data[index], sizeof(uint32_t), data.size(), stdout);
					break;
				}
				default:{
					std::cerr << "Invalid number of output bits: supported formats are 8/16/32 bits";
					break;
				}
			}
		}
	}
}