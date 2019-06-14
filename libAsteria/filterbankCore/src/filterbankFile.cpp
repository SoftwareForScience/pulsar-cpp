#include "filterbankCore.hpp"

filterbank filterbank::read_file(std::string filename) {
	filterbank fb = filterbank();

	fb.infilename = filename;
	auto inf = fopen(filename.c_str(), "rb");

	if (inf == NULL) {
		std::cerr << "Failed to read from file \n";
	}

	if (fb.read_header_file(inf)) {
		throw "Invalid filterbank file";
	}


	return fb;
}


void filterbank::write_file(std::string filename, bool headerless) {
	auto fp = fopen(filename.c_str(), "wb");
	if (fp == NULL) {
		std::cerr << "Failed to open file for writing: " << filename << std::endl;
		return;
	}

	if (!headerless) {
		write_string("HEADER_START");
		for (auto param : header)
		{
			//Skip unused headers
			if (param.second.val.d == 0.0) {
				continue;
			}
			switch (param.second.type) {
			case INT: {
				write_value(param.first, param.second.val.i);
				break;
			}
			case DOUBLE: {
				write_value(param.first, param.second.val.d);
				break;
			}
			case STRING: {
				write_string(param.first);
				write_string(param.second.val.s);
				break;
			}
			}
		}
		write_string("HEADER_END");
	}

	for (uint32_t sample = 0; sample < n_samples; ++sample) {
		for (uint32_t interface = 0; interface < n_ifs; ++interface) {
			// Get the index for the interface
			unsigned int index = (sample * n_ifs * n_channels) + (interface * n_channels);
			switch (n_bytes) {
			case 1: {
				std::vector<uint8_t>cwbuf(n_channels);
				for (unsigned int channel = start_channel; channel < end_channel; channel++) {
					cwbuf[channel - start_channel] = (uint8_t)data[((uint64_t)index) + channel];
				}

				fwrite(&cwbuf[0], sizeof(uint8_t), cwbuf.size(), outf);
				break;
			}
			case 2: {
				std::vector<uint16_t>swbuf(n_channels);
				for (unsigned int channel = start_channel; channel < end_channel; channel++) {
					swbuf[channel - start_channel] = (uint16_t)data[((uint64_t)index) + channel];
				}

				fwrite(&swbuf[0], sizeof(uint16_t), swbuf.size(), outf);
				break;
			}
			case 4: {
				fwrite(&data[index], sizeof(uint32_t), data.size(), outf);
				break;
			}
			}
		}
	}
	fclose(outf);
}

bool filterbank::read_header_file(FILE* fp) {
	if (fp == NULL) {
		return false;
	}

	uint32_t keylen = 0;
	char* buffer = read_string(fp, keylen);
	const std::string initial(buffer);
	
	if (initial.compare("HEADER_START")) {
		std::cerr << "Error, File is not a valid filterbank file";
		return false;
	}

	while (true) {
		buffer = read_string(fp, keylen);
		const std::string token(buffer);
		if (!token.compare("HEADER_END")) {
			// get size of the header by getting the current position;
			header_size = ftell(fp);
			break;
		}

		switch (header[token].type) {
		case INT: {
			header[token].val.i = read_value<int>();
			break;
		}
		case DOUBLE: {
			header[token].val.d = read_value<double>();
			break;
		}
		case STRING: {
			auto value = read_string(fp, keylen);
			//TODO: Error handling
			strncpy(header[token].val.s, value, keylen);
			break;
		}
		};

	}

	// get the size of the file by looking for the end, then getting the current position;
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
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

uint32_t filterbank::read_key_size(FILE* fp) {
	uint32_t keylen;
	fread(&keylen, sizeof(keylen), 1, fp);
	return keylen;
}

char* filterbank::read_string(FILE* fp, uint32_t& keylen) {
	fread(&keylen, sizeof(uint32_t), 1, fp);
	char* buffer = new char[(((uint64_t)keylen) + 1)]{ '\0' };
	std::fread(buffer, sizeof(char), keylen, fp);
	return buffer;
}

void filterbank::write_string(FILE* fp, const std::string string) {
	uint32_t len = string.length();
	//Write the length of our string
	fwrite(&len, sizeof(int), 1, fp);
	//then write the actual string
	fwrite(string.c_str(), sizeof(char), len, fp);
	fflush(fp);
}

template <typename T >
T filterbank::read_value(FILE* fp) {
	T value;
	fread(&value, sizeof(T), 1, fp);
	return value;
}

template <typename T>
void filterbank::write_value(FILE* fp, const std::string key, T value) {
	write_string(key);
	fwrite(&value, sizeof(T), 1, fp);
	fflush(fp);
}


