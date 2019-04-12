#ifndef FILTERBANK_H
#define FILTERBANK_H

#include <map>
#include <string>
#include <map>
#include <fstream>
#include "headerparam.h"

const std::map<uint16_t, std::string> telescope_ids = {
	{ 0, "Fake" }, { 1, "Arecibo" }, { 1, "Ooty" },
	{ 3, "Nancay"}, { 4, "Parkes" },{ 5, "Jodrell"},
	{ 6, "GBT" }, { 7, "GMRT"},	{ 8, "Effelsberg"},
	{ 9, "ATA"}, { 10, "SRT"}, {11, "LOFAR"},
	{ 12, "VLA"}, { 20, "CHIME"}, {21, "FAST"},
	{64, "MeerKAT" }, {65, "KAT-7"}
};

const std::map<uint16_t, std::string> machine_ids = {
	{0,"FAKE"}, {1, "PSPM"}, {2, "WaPP"},
	{3, "AOFTM"}, {4,"BCPM1"}, {5, "OOTY"},
	{6, "SCAMP"}, {7, "SPIGOT"}, {11, "BG/P"},
	{12, "PDEV"}, {20, "CHIME+PSR"}, {64, "KAT"},
	{65, "KAT-DC2"}
};



class filterbank {
public:
	filterbank(std::string filename);

	std::map<std::string, header_param> header
	{
		{"telescope_id", INT},
		{"machine_id", INT},
		{"data_type", INT},
		{"rawdatafile", STRING},
		{"source_name", STRING},
		{"barycentric", INT},
		{"pulsarcentric", INT},
		{"az_start", DOUBLE},
		{"za_start", DOUBLE},
		{"src_raj", DOUBLE},
		{"src_dej", DOUBLE},
		{"tstart", DOUBLE},
		{"tsamp", DOUBLE},
		{"nbits", INT},
		{"nsamples", INT},
		{"fch1", DOUBLE},
		{"foff", DOUBLE},
		{"nchans", INT},
		{"nifs", INT},
		{"refdm", DOUBLE},
		{"period", DOUBLE},
		{"nbeams", INT},
		{"ibeam", INT}
	};

private:
	unsigned int read_key_size(std::ifstream& f);
	bool read_header(std::ifstream& f);
	int read_int(std::ifstream& f);
	double read_double(std::ifstream& f);
	char* read_string(std::ifstream& f, int* len);

	std::string filename;
	std::ifstream ifs;
};

#endif // !FILTERBANK_H
