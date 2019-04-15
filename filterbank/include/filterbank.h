#ifndef FILTERBANK_H
#define FILTERBANK_H

#include <map>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <list>
#include "headerparam.h"
#include "Eigen/Dense"

class filterbank {
public:
	filterbank();
	static filterbank* read_filterbank(std::string filename);
	void save_filterbank(std::string filename);

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
		{"fch1", DOUBLE}, // centre frequency of first filterbank channel
		{"foff", DOUBLE}, // filterbank channel bandwith
		{"nchans", INT}, // number of filterbank channels
		{"nifs", INT}, // number of seperate if channels
		{"refdm", DOUBLE}, // reference dispersion measure
		{"period", DOUBLE}, // folding period (s)
		{"nbeams", INT},
		{"ibeam", INT}
	};

	unsigned int header_size = 0;
	unsigned int data_size = 0;

	bool read_header();
	bool read_data();

	void setup_time(unsigned int start_sample, unsigned int end_sample);
	void setup_frequencies(unsigned int startchannel, unsigned int endchannel);
	
	std::string telescope;
	std::string backend;

	std::list<double> timestamps;
	std::list<double> frequencies;
	Eigen::MatrixXd data;

private:
	unsigned int read_key_size(std::ifstream& f);
	template <typename T>
	T read_value(std::ifstream& f);
	char* read_string(std::ifstream& f, int* len);

	std::string filename;
	std::ifstream f;

	unsigned int n_bytes = 0;
	unsigned int file_size = 0;
	double center_freq = 0.0;

	unsigned int n_samples = 0;
	unsigned int n_channels = 0;

	static std::map<uint16_t, std::string> telescope_ids;
	static std::map<uint16_t, std::string> machine_ids;

};

#endif // !FILTERBANK_H
