#include "filterbank.h"

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

filterbank filterbank::read_filterbank(std::string filename) {
	filterbank fb;
	clock_t time_req = clock();

	fb.filename = filename;


	if (!fb.read_header()) {
		throw "Invalid filterbank file";
	}

	fb.setup_time(false, false);
	time_req = clock() - time_req;
	std::cout << "Time spent setting time: " << time_req / CLOCKS_PER_SEC << " seconds\n";

	fb.setup_frequencies(false, false);
	time_req = clock() - time_req;
	std::cout << "Time spent setting up channels: " << time_req / CLOCKS_PER_SEC << " seconds\n";
	return fb;
}

void filterbank::save_filterbank() {
	f = fopen(filename.c_str(), "wb");

	if (f == NULL)
		std::cout << "Failed to write to file \n";

	write_string("HEADER_START");
	for each (auto param in header)
	{
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
			write_value(param.first, param.second.val.s);
			break;
		}
		}
	}
	write_string("HEADER_END");

	for (unsigned int sample = 0; sample < n_samples; ++sample) {
		for (unsigned int interface = 0; interface < n_ifs; ++interface) {
			int index = (sample * n_ifs * n_channels) + (interface * n_channels);
			switch (n_bytes) {
			case 1: {
				char* cwbuf = new char[n_channels];
				// Get the index for the interface
				
				for (unsigned int channel; channel < n_channels; channel++) {
					cwbuf[channel] = (char) data[index + channel];
				}
							
				fwrite(cwbuf, sizeof(char), n_channels, f);
				break;
			}
			case 2: {
				uint16_t* swbuf = new uint16_t[n_channels];
				for (unsigned int channel; channel < n_channels; channel++) {
					swbuf[channel] = (unsigned short)data[index + channel];
				}

				fwrite(swbuf, sizeof(uint16_t), n_channels, f);

				break;
			}
			case 4: {
				fwrite(&data[index], sizeof(float), n_channels, f);
				break;
			}
			}
		}
	}

	fclose(f);
}

filterbank::filterbank() {
	f = NULL;
}

bool filterbank::read_header() {
	f = fopen(filename.c_str(), "rb");

	if (f == NULL) {
		return false;
	}

	int keylen = 0;
	char* buffer = read_string(keylen);
	const std::string initial(buffer);

	// char* buffer = new char[keylen] { '\0' };
	// f.read(buffer, sizeof(char) * keylen);

	if (initial.compare("HEADER_START")) {
		// if this isn't present, the file is not a valid filterbank file.
		return false;
	}

	while (true) {
		int keylen = 0;
		buffer = read_string(keylen);
		const std::string token(buffer);
		// buffer = new char[keylen + 1]{ '\0' };
		// f.read(buffer, sizeof(char) * keylen);
		if (!token.compare("HEADER_END")) {
			// get size of the header by getting the current position;
			header_size = unsigned int(ftell(f));

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
			int keylen;
			auto value = read_string(keylen);
			strncpy_s(header[token].val.s, value, keylen);
			break;
		}
		};

	}

	// get the size of the file by looking for the end, then getting the current position;
	fseek(f, 0, SEEK_END);
	file_size = unsigned int(ftell(f));
	data_size = file_size - header_size;
	
	n_channels = header["nchans"].val.i;
	n_ifs = header["nifs"].val.i;
	center_freq = (header["fch1"].val.d + n_channels * header["foff"].val.d / 2.0);
	n_bytes = header["nbits"].val.i / 8;

	telescope = telescope_ids[header["telescope_id"].val.i];
	backend = machine_ids[header["machine_id"].val.i];

	// if nsamples isn't set, get it from the data size
	if (!header["nsamples"].val.i) {
		header["nsamples"].val.i =   data_size / (n_bytes * n_channels * n_ifs);
	}

	n_samples = header["nsamples"].val.i;
	n_values = n_ifs * n_channels * n_samples;

	fclose(f);

	return true;
}

bool filterbank::read_data() {
	if (f == NULL)
		return false;

	f = fopen(filename.c_str(), "rb");

	
	fseek(f, header_size -1, SEEK_SET);

	// Allocate a block of data
	data = new float[n_values];
	read_block(header["nbits"].val.i, data, n_values);

	fclose(f);

	return true;
}

int filterbank::read_block(int nbits, float* block, int nread) {
	int samples_read = 0;
	unsigned char* charblock = nullptr;
	unsigned short* shortblock = nullptr;
	int sample = 0;

	/* decide how to read the data based on the number of bits per sample */
	switch (nbits) {
	case 1:	/* read n/8 bytes into character block containing n 1-bit pairs */
		charblock = new unsigned char[nread / 8];
		samples_read = fread(charblock, 1, nread, f);
		for (int i = 0; i < samples_read; i++) {
			for (int j = 0; j < 8; j++) {
				block[sample++] = charblock[i] & 1;
				charblock[i] >>= 1;
			}
		}
		samples_read = sample;
		break;

	case 2:	/* read n/4 bytes into character block containing n 2-bit pairs */
		charblock = new unsigned char[nread / 4];
		samples_read = fread(charblock, 1, nread / 4, f);

		for (int i = 0; i < samples_read; i++) {
			block[sample++] = charblock[i] & 3;
			block[sample++] = charblock[i] & 12;
			block[sample++] = charblock[i] & 48;
			block[sample++] = charblock[i] & 192;
		}
		samples_read *= 4;

	case 4:	/* read n/2 bytes into character block containing n 4-bit pairs */
		charblock = new unsigned char[nread / 2];
		samples_read = fread(charblock, 1, nread / 2, f);
		for (int i = 0; i < samples_read; i++) {
			block[sample++] = (charblock[i] & 15);
			block[sample++] = (charblock[i] & 240) >> 4;
		}
		samples_read *= 2;
		break;

	case 8: /* read n bytes into character block containing n 1-byte numbers */
		charblock = new unsigned char[nread];
		samples_read = fread(charblock, 1, nread, f);
		for (int i = 0; i < samples_read; i++) {
			block[i] = (float)charblock[i];
		}
		break;

	case 16: /* read 2*n bytes into short block containing n 2-byte numbers */
		shortblock = new unsigned short[(nread * 2)];
		samples_read = fread(shortblock, 2, nread, f);
		for (int i = 0; i < samples_read; i++) {
			block[i] = (float)shortblock[i];
		}
		break;
	case 32:
		samples_read = fread(block, sizeof(float), nread, f);
		auto eof = feof(f);
		auto err = ferror(f);

		break;
	}

	delete[] charblock;
	delete[] shortblock;

	return samples_read;

}

void filterbank::setup_frequencies(unsigned int startchan, unsigned int endchan) {
	double channel_bandwith = header["foff"].val.d;
	double start_frequency = header["fch1"].val.d;

	double f_start = startchan
		? ((startchan - start_frequency) / channel_bandwith)
		: 0;

	double f_stop = endchan
		? ((endchan - start_frequency) / channel_bandwith)
		: header["nchans"].val.i;


	for (unsigned int i = 0; i < n_channels; i++) {
		frequencies.push_back(i * channel_bandwith + start_frequency);
	}

	n_channels = int(f_stop - f_start);
	start_channel = std::min(f_start, f_stop);
	end_channel = std::max(f_start, f_stop);

	//recalculate number of values
	n_values = n_ifs * n_channels * n_samples;
}

void  filterbank::setup_time(unsigned int start, unsigned int end) {
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

	for (int i = 0; i < n_samples; i++) {
		timestamps.push_back(i * sample_interval / 24. / 60. / 60 + time_start);
	}

	// Recalculate number of values 
	n_values = n_ifs * n_channels * n_samples;

}

unsigned int filterbank::read_key_size() {
	unsigned int keylen;
	fread(&keylen, sizeof(keylen), 1, f);
	return keylen;
}

char* filterbank::read_string(int& keylen) {
	fread(&keylen, sizeof(uint32_t), 1, f);
	char* buffer = new char[(keylen + 1)]{ '\0' };
	fread(buffer, sizeof(char), keylen, f);
	return buffer;
}

void filterbank::write_string(char* string) {
	int len = strlen(string);
	//Write the length of our string
	fwrite(&len, sizeof(int), 1, f);
	//then write the actual string
	fwrite(string, sizeof(char), len, f);
}

template <typename T >
T filterbank::read_value() {
	T value;
	fread(&value, sizeof(T), 1, f);
	return value;
}

template <typename T>
void filterbank::write_value(std::string key, T value) {
	//If there's a key, associated with the value(eg: header parameters) write the key
	std::cout << "Key:\t"<< key.c_str() << "\t\t Value:\t" << value << "\n"; 

	if (key.compare("")) {
		write_string((char*)key.c_str());
	}
	fwrite(&value, sizeof(T), 1, f);


}
