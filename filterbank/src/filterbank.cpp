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

filterbank::filterbank(std::string filename) {

	this->filename = filename;
	f.open(this->filename, std::ifstream::in);

	if (!read_header()) {
		throw 1;
	}

	n_samples = setup_time(false, false);
	n_channels = setup_frequencies(false, false);
	read_filterbank();
}

std::string filterbank::telescope() {
	return telescope_ids[header["telescope_id"].val.i];
}

std::string filterbank::backend() {
	return machine_ids[header["machine_id"].val.i];
}

bool filterbank::read_header() {
	if (!f.is_open()) {
		return false;
	}

	unsigned int keylen = read_key_size(f);
	char* buffer = new char[keylen] { '\0' };

	f.read(buffer, sizeof(char) * keylen);
	if (!strcmp(buffer, "HEADER_START")) {
		// if this isn't present, the file is not a valid filterbank file.
		return false;
	}

	while (true) {
		keylen = read_key_size(f);
		buffer = new char[keylen + 1]{ '\0' };
		f.read(buffer, sizeof(char) * keylen);

		if (!strcmp(buffer, "HEADER_END\0")) {
			break;
		}
		const std::string token(buffer);

		switch (header[token].type) {
		case INT: {
			header[token].val.i = read_value<int>(f);
			break;
		}
		case DOUBLE: {
			header[token].val.d = read_value<double>(f);
			break;
		}
		case STRING: {
			int len;
			auto value = read_string(f, &len);
			strncpy_s(header[token].val.s, value, len);
			break;
		}
		};

	}

	// get size of the header by getting the current position;
	header_size = unsigned int(f.tellg());
	// get the size of the file by looking for the end, then getting the current position;
	f.seekg(0, f._Seekend);
	file_size = unsigned int(f.tellg());
	data_size = file_size - header_size;

	//Move back to the original position to start reading the body;
	f.seekg(header_size);

	center_freq = (header["fch1"].val.d + header["nchans"].val.i * header["foff"].val.d / 2.0);
	n_bytes = header["nbits"].val.i / 8;

	return true;
}

bool filterbank::read_filterbank() {
	if (!f.is_open()) {
		return false;
	}

	// Allocate a block of data
	data.resize(n_channels, n_samples);

	for (int channel = 0; channel < n_channels; channel++) {
		for (int sample = 0; sample < n_samples; sample++) {
			f.seekg((uint64_t)n_bytes * (uint64_t)channel, 1);
			switch (n_bytes) {
			case 1: {
				data(channel, sample) = read_value<unsigned char>(f);
				break;
			}
			case 2: {
				data(channel, sample) = read_value<unsigned short>(f);
				break;
			}
			case 4: {
				data(channel, sample) = read_value<float>(f);
				break;
			}
			}

		}
	}
	return true;
}

int filterbank::setup_frequencies(int startFreq = 0, int endFreq = 0) {
	double channel_bandwith = header["foff"].val.d;
	double start_frequency = header["fch1"].val.d;

	double f_start = startFreq
		? startFreq = int((startFreq - start_frequency) / channel_bandwith)
		: 0;

	double f_stop = endFreq
		? ((endFreq - start_frequency) / channel_bandwith)
		: header["nchans"].val.i;

	int n_frequencies = int(f_stop - f_start);


	for (int i = 0; i < n_frequencies; i++) {
		frequencies.push_back(i * channel_bandwith + start_frequency);
	}

	return n_frequencies;
}

int  filterbank::setup_time(int start_sample = 0, int end_sample = 0) {
	double sample_interval = header["tsamp"].val.d;
	double time_start = header["tstart"].val.d;

	if (!end_sample) {
		end_sample = data_size / (n_bytes * header["nchans"].val.i * header["nifs"].val.i);
	}

	int n_samples = end_sample - start_sample;

	for (int i = 0; i < n_samples; i++) {
		timestamps.push_back(i * sample_interval / 24. / 60. / 60 + time_start);
	}
	return n_samples;
}

unsigned int filterbank::read_key_size(std::ifstream& f) {
	unsigned int keylen;
	f.read(reinterpret_cast<char*>(&keylen), sizeof(keylen));
	return keylen;
}

char* filterbank::read_string(std::ifstream & f, int* len) {
	unsigned int strlen;
	f.read(reinterpret_cast<char*>(&strlen), sizeof(strlen));
	char* buffer = new char[strlen + 1]{ '\0' };
	f.read(buffer, sizeof(char) * strlen);
	return buffer;
}

template <typename T >
T filterbank::read_value(std::ifstream & f) {
	T value;
	f.read(reinterpret_cast<char*>(&value), sizeof(value));
	return value;
}

