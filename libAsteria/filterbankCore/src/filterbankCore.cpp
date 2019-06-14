#include "filterbankCore.hpp"

std::map<uint16_t, std::string> filterbank::telescope_ids = {
	{ 0, "Fake" }, { 1, "Arecibo" }, { 1, "Ooty" },
	{ 3, "Nancay"}, { 4, "Parkes" },{ 5, "Jodrell"},
	{ 6, "GBT" }, { 7, "GMRT"},	{ 8, "Effelsberg"},
	{ 9, "ATA"}, { 10, "SRT"}, {11, "LOFAR"},
	{ 12, "VLA"}, { 20, "CHIME"}, {21, "FAST"},
	{64, "MeerKAT" }, {65, "KAT-7"}
};

std::map<uint16_t, std::string> filterbank::machine_ids = {
	{0,"FAKE"}, {1, "PSPM"}, {2, "WaPP"},
	{3, "AOFTM"}, {4,"BCPM1"}, {5, "OOTY"},
	{6, "SCAMP"}, {7, "SPIGOT"}, {11, "BG/P"},
	{12, "PDEV"}, {20, "CHIME+PSR"}, {64, "KAT"},
	{65, "KAT-DC2"}
};


filterbank filterbank::read(filterbank::inputType inType, std::string input = "") {
	switch (inType) {
	case inputType::STDIO:
		return read_stdio(input);
	case inputType::INFILE:
		return read_file(input);
	}
}

filterbank filterbank::read_stdio(std::string input) {
	std::string tempString;

	while (std::cin.good())
	{
		auto tempChar = std::cin.get();
		if (std::cin.good()) {
			tempString += tempChar;
		}
	}
}


void filterbank::write(std::string filename = "", bool headerless = false) {

}


filterbank filterbank::read_file(std::string filename) {
	FILE *inf;
	filterbank fb;
	clock_t time_req = clock();

	fb.infilename = filename;
	inf = fopen(filename.c_str(), "rb");
	
	if (inf == NULL) {
		std::cerr << "Failed to read from file \n";
	}

	if (1) {
		throw "Invalid filterbank file";
	}
	fb.setup_time(false, false);
	time_req = clock() - time_req;
	// std::cout << "Time spent setting time: " << time_req / CLOCKS_PER_SEC << " seconds\n";

	fb.setup_frequencies(false, false);
	time_req = clock() - time_req;
	// std::cout << "Time spent setting up channels: " << time_req / CLOCKS_PER_SEC << " seconds\n";
	return fb;
}

void filterbank::save_file(std::string filename, bool save_header) {
	bool cmdOutput = false;
	if (outfilename.compare("") == 0) {
		std::FILE* tmpf = std::tmpfile();
		outf = tmpf;
		cmdOutput = true;
	}
	else {
		outf = fopen(outfilename.c_str(), "wb");
	}

	//TODO: Error handling
	if (outf == NULL) {
		std::cerr << "Failed to write to file \n";
		return;
	}

	if (save_header) {
		//TODO: Pointers? Rewrite to single method for any int/double/string?
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
			int32_t index = (sample * n_ifs * n_channels) + (interface * n_channels);
			switch (n_bytes) {
			case 1: {
				std::vector<uint8_t>cwbuf(n_channels);
				for (uint32_t channel = start_channel; channel < end_channel; channel++) {
					cwbuf[channel - start_channel] = (uint8_t)data[((uint64_t)index) + channel];
				}

				fwrite(&cwbuf[0], sizeof(uint8_t), cwbuf.size(), outf);
				break;
			}
			case 2: {
				std::vector<uint16_t>swbuf(n_channels);
				for (uint32_t channel = start_channel; channel < end_channel; channel++) {
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


	if (cmdOutput == true) {
		char s;
		std::rewind(outf);
		while ((s = fgetc(outf)) != EOF) {
			std::cout << s;
		}
	}

	fclose(inf);
	fclose(outf);
}

filterbank::filterbank() {
	data = std::vector<float>(0);
}

bool filterbank::read_header_file() {
	//TODO: Error handling
	// f = fopen(filename.c_str(), "rb");
	if (inf == NULL) {
		return false;
	}

	uint32_t keylen = 0;
	char* buffer = read_string(keylen);
	const std::string initial(buffer);
	if (initial.compare("HEADER_START")) {
		// if this isn't present, the file is not a valid filterbankCore file.
		return false;
	}
	while (true) {
		buffer = read_string(keylen);
		const std::string token(buffer);
		if (!token.compare("HEADER_END")) {
			// get size of the header by getting the current position;
			header_size = ftell(inf);
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
			auto value = read_string(keylen);
			//TODO: Error handling
			strncpy(header[token].val.s, value, keylen);
			break;
		}
		};

	}

	// get the size of the file by looking for the end, then getting the current position;
	fseek(inf, 0, SEEK_END);
	file_size = ftell(inf);
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

	// fclose(f);

	return true;
}

bool filterbank::read_data() {
	//TODO: Error handling
	// f = fopen(filename.c_str(), "rb");
	auto n_bytes_read = 0;

	if (inf == NULL) {
		return false;
	}

	// Allocate a block of data
	data = std::vector<float>(n_values);

	// Skip the header
	fseek(inf, header_size, SEEK_SET);

	// Skip the amount of samples we're offset from
	auto n_bytes_to_skip = (start_sample * n_ifs * header["nchans"].val.i);
	fseek(inf, n_bytes_to_skip, SEEK_CUR);

	for (uint32_t sample = 0; sample < n_samples; sample++) {
		for (uint32_t interface = 0; interface < n_ifs; interface++) {
			int32_t start_bytes_to_skip = start_channel * n_bytes;
			int32_t end_bytes_to_skip = (header["nchans"].val.i - end_channel) * n_bytes;

			//Skip the amount of channels we're not interested in
			fseek(inf, start_bytes_to_skip, SEEK_CUR);
			n_bytes_read += read_block(header["nbits"].val.i, &data[n_bytes_read], n_channels);
			//Skip the last few channels
			fseek(inf, end_bytes_to_skip, SEEK_CUR);
		}
	}

	// fclose(f);

	return true;
}

uint32_t filterbank::read_data(uint16_t nbits, float* block, const uint32_t nread) {
	size_t samples_read = 0;
	uint32_t sample = 0;
	std::vector<uint8_t> charblock(nread);
	std::vector<uint16_t> shortblock(nread);

	/* decide how to read the data based on the number of bits per sample */
	switch (nbits) {
	case 8: /* read n bytes into character block containing n 1-byte numbers */
		samples_read = fread(&charblock[0], sizeof(uint8_t), charblock.size(), inf);
		for (uint32_t i = 0; i < nread; i++) {
			block[i] = (float)charblock[i];
		}
		break;

	case 16: /* read 2*n bytes into short block containing n 2-byte numbers */
		samples_read = fread(&shortblock[0], sizeof(uint16_t), shortblock.size(), inf);
		for (uint32_t i = 0; i < samples_read; i++) {
			block[i] = (float)shortblock[i];
		}
		break;
	case 32:
		samples_read = fread(block, sizeof(float), nread, inf);
		break;
	}

	return (uint32_t)samples_read;

}

void filterbank::setup_frequencies(uint32_t startchan, uint32_t endchan) {
	double channel_bandwith = header["foff"].val.d;
	double start_frequency = header["fch1"].val.d;

	double f_start = startchan
		? ((startchan - start_frequency) / channel_bandwith)
		: 0;

	double f_stop = endchan
		? ((endchan - start_frequency) / channel_bandwith)
		: header["nchans"].val.i;


	for (uint32_t i = 0; i < n_channels; i++) {
		frequencies.push_back(i * channel_bandwith + start_frequency);
	}

	n_channels = int(f_stop - f_start);
	start_channel = (uint32_t)std::min(f_start, f_stop);
	end_channel = (uint32_t)std::max(f_start, f_stop);

	//recalculate number of values
	n_values = n_ifs * n_channels * n_samples;
}

void  filterbank::setup_time(uint32_t start, uint32_t end) {
	double sample_interval = header["tsamp"].val.d;
	double time_start = header["tstart"].val.d;

	if (start > end) {
		auto temp = end;
		end_sample = start;
		start_sample = end;
	}

	// if end_sample is not set (0) or it is bigger than the number of samples in the file
	// Set the number of samples accordingly
	if (!end_sample || end_sample > n_samples) {
		end_sample = n_samples;
	}

	n_samples = end_sample - start_sample;

	for (uint32_t i = 0; i < n_samples; i++) {
		timestamps.push_back(i * sample_interval / 24. / 60. / 60 + time_start);
	}

	// Recalculate number of values 
	n_values = n_ifs * n_channels * n_samples;

}

uint32_t filterbank::read_key_size() {
	uint32_t keylen;
	fread(&keylen, sizeof(keylen), 1, inf);
	return keylen;
}

char* filterbank::read_string(uint32_t& keylen) {
	fread(&keylen, sizeof(uint32_t), 1, inf);

	char* buffer = new char[(((uint64_t)keylen) + 1)]{ '\0' };
	std::fread(buffer, sizeof(char), keylen, inf);
	return buffer;
}

void filterbank::write_string(const std::string string) {
	uint32_t len = string.length();
	//Write the length of our string
	fwrite(&len, sizeof(int), 1, outf);
	//then write the actual string
	fwrite(string.c_str(), sizeof(char), len, outf);
	fflush(outf);
}

template <typename T >
T filterbank::read_value() {
	T value;
	fread(&value, sizeof(T), 1, inf);
	return value;
}

template <typename T>
void filterbank::write_value(const std::string key, T value) {
	write_string(key);
	fwrite(&value, sizeof(T), 1, outf);
	fflush(outf);
}
