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
#include <stdio.h>
#include "headerParam.hpp"

class filterbank {
public:
	filterbank();

	enum ioType
	{
		STDIO = 0,
		FILEIO = 1
	};


	static filterbank read(filterbank::ioType inputType, std::string input = "");


	// If we receive an empty string, write to STDIO
	static void write(filterbank::ioType outputType, std::string filename = "", bool headerless = false);


	void save_file(std::string filename, bool save_header = true);


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
		{"fch1", DOUBLE}, // centre frequency of first filterbankCore channel
		{"foff", DOUBLE}, // filterbankCore channel bandwith
		{"nchans", INT}, // number of filterbankCore channels
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


	bool read_data();


	void setup_time(uint32_t start, uint32_t end);
	void setup_frequencies(uint32_t startchan, uint32_t endchan);

	std::string telescope;
	std::string backend;

	std::list<double> timestamps;
	std::list<double> frequencies;
	std::vector<float> data;
	std::string infilename;
	std::string outfilename;

private:
	static filterbank read_stdio(std::string input);
	static filterbank read_file(std::string filename);
	bool read_header_file(FILE* inf);
	bool read_header_stdio(FILE* inf);
	bool write_file(std::string filename, bool headerless);
	bool write_stdio(bool headerless);

	uint32_t n_bytes = 0;
	uint32_t file_size = 0;
	double center_freq = 0.0;

	static std::map<uint16_t, std::string> telescope_ids;
	static std::map<uint16_t, std::string> machine_ids;

	template <typename T>
	T read_value(FILE* fp);
	
	uint32_t read_key_size(FILE* fp);
	char* read_string(FILE* fp, uint32_t& keylen);
	uint32_t read_data(FILE* fp, uint16_t nbits, float* block, uint32_t nread);

	template <typename T>
	void write_value(FILE* fp, std::string key, T value);
	void write_string(FILE* fp, std::string string);
};

#endif // !FILTERBANK_H