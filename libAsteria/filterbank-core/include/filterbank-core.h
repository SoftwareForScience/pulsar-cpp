#ifndef FILTERBANK_H
#define FILTERBANK_H

#include <algorithm>
#include <map>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <list>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <vector>
#include "HeaderParam.h"
#include <vector>

class filterbank {
public:
	filterbank();
	static filterbank read_filterbank(std::string filename);
	void save_filterbank(bool save_header = true);

	std::map<std::string, header_param> header
	{
		{"telescope_id", INT},
		{"machine_id", INT},
		{"data_type", INT},
		{"rawdatafile", STRING}, // name of the original data file
		{"source_name", STRING}, // the name of the source being observed by the telescope
		{"barycentric", INT},
		{"pulsarcentric", INT},
		{"az_start", DOUBLE}, // telescope azimut at start of scan
		{"za_start", DOUBLE}, // telescope zenith angle at start of scan
		{"src_raj", DOUBLE}, // right ascension of source (hhmmss.s)
		{"src_dej", DOUBLE}, // declination of source (ddmmss.s)
		{"tstart", DOUBLE}, // time stamp of first sample
		{"tsamp", DOUBLE}, // time interval between samples
		{"nbits", INT}, // number of bits per time sample
		{"nsamples", INT}, // number of time samples in the data file
		{"fch1", DOUBLE}, // centre frequency of first filterbank-core channel
		{"foff", DOUBLE}, // filterbank-core channel bandwith
		{"nchans", INT}, // number of filterbank-core channels
		{"nifs", INT}, // number of seperate if channels
		{"refdm", DOUBLE}, // reference dispersion measure
		{"period", DOUBLE}, // folding period (s)
		{"nbeams", INT},
		{"ibeam", INT}
	};

	uint32_t header_size = 0;
	uint32_t data_size = 0;
	uint32_t n_values = 0;
	uint32_t n_samples = 0;
	uint32_t n_channels = 0;
	uint32_t n_ifs = 0;
	uint32_t start_channel = 0;
	uint32_t start_sample = 0;
	uint32_t end_channel = 0;
	uint32_t end_sample = 0;

	bool read_header();
	bool read_data();


	void setup_time(uint32_t start, uint32_t end);
	void setup_frequencies(uint32_t startchan, uint32_t endchan);

	std::string telescope;
	std::string backend;

	std::list<double> timestamps;
	std::list<double> frequencies;
	std::vector<float> data;
	std::string filename;

private:
	FILE* f;

	uint32_t n_bytes = 0;
	uint32_t file_size = 0;
	double center_freq = 0.0;

	static std::map<uint16_t, std::string> telescope_ids;
	static std::map<uint16_t, std::string> machine_ids;

	template <typename T>
	T read_value();

	uint32_t read_key_size();
	char* read_string(uint32_t& keylen);
	uint32_t read_block(uint16_t nbits, float* block, uint32_t nread);

	template <typename T>
	void write_value(std::string key, T value);
	void write_string(std::string string);
};

#endif // !FILTERBANK_H