#include "filterbankCore.hpp"

filterbank filterbank::read_file(std::string filename) {
	auto fb = filterbank();
	auto inf = fopen(filename.c_str(), "rb");

	if (inf == NULL) {
		std::cerr << "Failed to read from file \n";
	}

	if (!fb.read_header_file(inf)) {
		throw "Invalid filterbank file";
	}

	fb.read_data_file(inf);

	fclose(inf);
	return fb;
}

bool filterbank::read_data_file(FILE* fp) {
	size_t samples_read = 0;
	uint32_t sample = 0;

	if (fp == NULL) {
		return false;
	}

	// Allocate a block of data
	data = std::vector<float>(n_values);

	// Skip the header
	fseek(fp, header_size, SEEK_SET);

	auto nread = header["nsamples"].val.i * header["nifs"].val.i * header["nchans"].val.i;

	std::vector<uint8_t> charblock(nread);
	std::vector<uint16_t> shortblock(nread);

	/* decide how to read the data based on the number of bits per sample */
	switch (header["nbits"].val.i) {
		case 8: /* read n bytes into character block containing n 1-byte numbers */
			samples_read = fread(&charblock[0], sizeof(uint8_t), charblock.size(), fp);
			for (uint32_t i = 0; i < nread; i++) {
				data[i] = (float)charblock[i];
			}
			break;
		case 16: /* read 2*n bytes into short block containing n 2-byte numbers */
			samples_read = fread(&shortblock[0], sizeof(uint16_t), shortblock.size(), fp);
			for (uint32_t i = 0; i < samples_read; i++) {
				data[i] = (float)shortblock[i];
			}
			break;
		case 32:
			samples_read = fread(&data[0], sizeof(float), nread, fp);
			break;
		}
	return true;
}


bool filterbank::write_file(std::string filename, bool headerless) {
	auto fp = fopen(filename.c_str(), "wb");
	if (fp == NULL) {
		std::cerr << "Failed to open file for writing: " << filename << std::endl;
		return false;
	}

	if (!headerless) {
		write_string(fp, "HEADER_START");
		for (auto param : header) {
			//Skip unused headers
			if (param.second.val.d == 0.0) {
				continue;
			}
			switch (param.second.type) {
			case INT: {
				write_value(fp, param.first, param.second.val.i);
				break;
			}
			case DOUBLE: {
				write_value(fp, param.first, param.second.val.d);
				break;
			}
			case STRING: {
				write_string(fp, param.first);
				write_string(fp, param.second.val.s);
				break;
			}
			}
		}
		write_string(fp, "HEADER_END");
	}

	for (uint32_t sample = 0; sample < header["nsamples"].val.i; ++sample) {
		for (uint32_t interface = 0; interface < header["nifs"].val.i; ++interface) {
			// Get the index for the interface
			unsigned int index = (sample * header["nifs"].val.i * header["nchans"].val.i) + (interface * header["nchans"].val.i);
			switch (n_bytes) {
				case 1: {
					std::vector<uint8_t>cwbuf(header["nchans"].val.i);
					for (unsigned int channel = 0; channel < header["nchans"].val.i; channel++) {
						cwbuf[channel] = (uint8_t)data[((uint64_t)index) + channel];
					}

					fwrite(&cwbuf[0], sizeof(uint8_t), cwbuf.size(), fp);
					break;
				}
				case 2: {
					std::vector<uint16_t>swbuf(header["nchans"].val.i);
					for (unsigned int channel = 0; channel < header["nchans"].val.i; channel++) {
						swbuf[channel] = (uint16_t)data[((uint64_t)index) + channel];
					}

					fwrite(&swbuf[0], sizeof(uint16_t), swbuf.size(), fp);
					break;
				}
				case 4: {
					fwrite(&data[index], sizeof(uint32_t), data.size(), fp);
					break;
				}
			}
		}
	}
	fclose(fp);
}

bool filterbank::read_header_file(FILE* fp) {
	if (fp == NULL) {
		return false;
	}

	uint32_t keylen = 0;
	auto buffer = read_string(fp, keylen);
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
				header[token].val.i = read_value<int>(fp);
				break;
			}
			case DOUBLE: {
				header[token].val.d = read_value<double>(fp);
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

uint32_t filterbank::read_key_size(FILE* fp) {
	uint32_t keylen;
	fread(&keylen, sizeof(keylen), 1, fp);
	return keylen;
}

char* filterbank::read_string(FILE* fp, uint32_t& keylen) {
	fread(&keylen, sizeof(uint32_t), 1, fp);
	char* buffer = new char[(((uint64_t)keylen) + 1)]{ '\0' };
	fread(buffer, sizeof(char), keylen, fp);
	return buffer;
}

template <typename T >
T filterbank::read_value(FILE* fp) {
	T value;
	fread(&value, sizeof(T), 1, fp);
	return value;
}



