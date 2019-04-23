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

	fb.f = fopen(filename.c_str(), "r");

	if (!fb.read_header()) {
		throw "Invalid filterbank file";
	}

	fb.setup_time(false, false);
	time_req = clock() - time_req;
	std::cout << "Time spent setting time: " << time_req / CLOCKS_PER_SEC << " seconds\n";

	fb.setup_frequencies(false, false);
	time_req = clock() - time_req;
	std::cout << "Time spent setting up channels: " << time_req / CLOCKS_PER_SEC << " seconds\n";

	fb.read_data();
	time_req = clock() - time_req;
	std::cout << "Time spent reading data: " << time_req / CLOCKS_PER_SEC << " seconds\n";

	fclose(fb.f);

	return fb;
}

void filterbank::save_filterbank() {
	f = fopen(filename.c_str(), "w");

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


	for (unsigned int sample = 0; sample < header["nsamp"].val.i ; ++sample) {
		switch (n_bytes) {
		case 1: {
			char* datac = new char[n_channels];
			for (unsigned int i = 0; i < n_channels; i++) {
				datac[i] = data(sample, i);
			}

			fwrite(datac, sizeof(char), n_channels, f);

			break;
		}
		case 2: {
			uint16_t* datas = new uint16_t[n_channels];
			for (unsigned int i = 0; i < n_channels; i++) {
				datas[i] = data(sample, i);
			}

			fwrite(datas, sizeof(uint16_t), n_channels, f);

			break;
		}
		case 4: {
			float_t* datac = new float[n_channels];

			for (unsigned int i = 0; i < n_channels; i++) {
				datac[i] = data(sample, i);
			}

			fwrite(reinterpret_cast<char*>(datac), sizeof(float), n_channels, f);
			break;
		}
		}
	}

	fclose(f);
}

filterbank::filterbank() {
	f = NULL;
}

bool filterbank::read_header() {
	if (f == NULL) {
		return false;
	}

	//unsigned int keylen = read_key_size();
	unsigned int keylen;
	char* buffer = read_string(keylen);
	const std::string initial(buffer);

	// char* buffer = new char[keylen] { '\0' };
	// f.read(buffer, sizeof(char) * keylen);
	
	if (initial.compare("HEADER_START")) {
		// if this isn't present, the file is not a valid filterbank file.
		return false;
	}

	while (true) {
		keylen = 0;
		buffer = read_string(keylen);
		const std::string token(buffer);
		// buffer = new char[keylen + 1]{ '\0' };
		// f.read(buffer, sizeof(char) * keylen);
		if(!token.compare("HEADER_END")){
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
			unsigned int len;
			auto value = read_string(len);
			strncpy_s(header[token].val.s, value, len);
			break;
		}
		};

	}

	// get size of the header by getting the current position;
	header_size = unsigned int(ftell(f));
	// get the size of the file by looking for the end, then getting the current position;
	fseek(f, 0, SEEK_END);
	
	file_size = unsigned int(ftell(f));
	data_size = file_size - header_size;
	n_channels = header["nchans"].val.i;

	center_freq = (header["fch1"].val.d +  n_channels * header["foff"].val.d / 2.0);
	n_bytes = header["nbits"].val.i / 8;
	
	telescope = telescope_ids[header["telescope_id"].val.i];
	backend = machine_ids[header["machine_id"].val.i];

	// if nsamples isn't set, get it from the data size
	if (!header["nsamples"].val.i) {
		header["nsamples"].val.i = data_size / n_bytes * n_channels;
	}

	return true;
}

bool filterbank::read_data() {
	if (f == NULL)
		return false;
	fseek(f, header_size, SEEK_SET);

	// Allocate a block of data
	data.resize(header["nsamples"].val.i, n_channels);
	
	//if we've got an offset on the start, seek N bytes
	fseek(f, n_channels_offset * n_channels * n_bytes, SEEK_CUR);

	for (int sample = 0; sample < header["nsamples"].val.i ; sample++) {
		switch (n_bytes) {
		case 1: {
			char* datac = new char[n_channels];
			fread(datac, sizeof(char), n_channels, f);

			for (unsigned int i = 0; i< n_channels; i++) {
				data(sample, i) = datac[i];
			}
			break;
		}
		case 2: {
			unsigned short* datac = new unsigned short[n_channels];
			fread(reinterpret_cast<char*>(datac), sizeof(unsigned short), n_channels, f);

			for (unsigned int i = 0; i < n_channels; i++) {
				data(sample, i) = datac[i];
			}			break;
		}
		case 4: {
			float* datac = new float[n_channels];
			fread(reinterpret_cast<char*>(datac), sizeof(float), n_channels, f);

			for (unsigned int i = 0; i < n_channels; i++) {
				data(sample, i) = datac[i];
				std::cout << "index:\t" << i <<"\tdata: " << datac[i] << "\n";
			}
			break;
		}
		}
	}

	return true;
}

void filterbank::setup_frequencies(unsigned int startchannel = 0, unsigned int endchannel = 0) {
	double channel_bandwith = header["foff"].val.d;
	double start_frequency = header["fch1"].val.d;

	double f_start = startchannel
		? ((startchannel - start_frequency) / channel_bandwith)
		: 0;

	double f_stop = endchannel
		? ((endchannel - start_frequency) / channel_bandwith)
		: header["nchans"].val.i;

	n_channels = int(f_stop - f_start);

	if (startchannel != 0) {
		n_channels_offset = startchannel;
	}

	for (unsigned int i = 0; i < n_channels; i++) {
		frequencies.push_back(i * channel_bandwith + start_frequency);
	}
}

void  filterbank::setup_time(unsigned int start_sample = 0, unsigned int end_sample = 0) {
	double sample_interval = header["tsamp"].val.d;
	double time_start = header["tstart"].val.d;

	if (!end_sample) {
		end_sample = data_size / (n_bytes * header["nchans"].val.i * header["nifs"].val.i);
	}

	int n_samples = end_sample - start_sample;

	for (int i = 0; i < n_samples; i++) {
		timestamps.push_back(i * sample_interval / 24. / 60. / 60 + time_start);
	}

}

unsigned int filterbank::read_key_size() {
	unsigned int keylen;
	fread(&keylen, sizeof(keylen), 1, f);
	return keylen;
}

char* filterbank::read_string(unsigned int& len) {
	int strlen = 0;
	fread(&strlen, sizeof(uint32_t), 1, f);
	char* buffer = new char[strlen + 1]{ '\0' };	
	fread(buffer, sizeof(char), strlen, f);
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
	if (key.compare("")) {
		write_string((char*)key.c_str());
	}
	fwrite(&value, sizeof(T), 1, f);
}


