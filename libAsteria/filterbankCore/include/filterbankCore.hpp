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
		void write(filterbank::ioType outputType, std::string filename = "", bool headerless = false);

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

	std::string telescope;
	std::string backend;

	uint32_t header_size = 0;
	uint32_t data_size = 0;

	std::vector<float> data;

private:
	static filterbank read_stdio(std::string input);
	bool read_header_stdio(std::string input);
	bool read_data_stdio(std::string input);

	static filterbank read_file(std::string filename);
	bool read_header_file(FILE* inf);
	bool read_data_file(FILE* inf);

	bool write_file(std::string filename, bool headerless);
	bool write_stdio(bool headerless);


	uint32_t n_values = 0;
	uint32_t n_bytes = 0;
	uint32_t file_size = 0;

	double center_freq = 0.0;

	static std::map<uint16_t, std::string> telescope_ids;
	static std::map<uint16_t, std::string> machine_ids;


	
	uint32_t read_key_size(FILE* fp);
	uint32_t read_data(FILE* fp);

	template <typename T>
	T read_value(FILE* fp);
	template <typename T>
	void write_value(FILE* fp, std::string key, T value);

	char* read_string(FILE* fp, uint32_t& keylen);
	void write_string(FILE* fp, std::string string);
};

#endif // !FILTERBANK_H