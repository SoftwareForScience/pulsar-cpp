#ifndef FILTERBANK_HEADER_H
#define FILTERBANK_HEADER_H
#include <iostream>
#include <string> 
#include <fstream>		
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include "filterbank.h"

class filterbank_header {
public:
	filterbank_header(std::string filename);

	int telescope_id = 0;
	int machine_id = 0;
	int data_type = 0 ;
	std::string rawdatafile = ""; // name of the ooriginal data file
	std::string source_name = ""; // the name of the source being observed by the telescope
	int barycentric = 0;
	int pulsarcentric = 0;
	double az_start = .0; // telescope azimut at start of scan
	double za_start = .0 ; // telescope zenith angle at start of scan
	double src_raj = .0; // right ascension of source (hhmmss.s)
	double src_dej = .0; // declination of source (ddmmss.s)
	double tstart = .0; // time stamp of first sample
	double tsamp = .0; // time interval between samples
	int nbits = 0; // number of bits per time samples
	int nsamples = 0; // number of time samples in the data file
	double fch1 = .0; // centre frequency of first filterbank channel
	double foff = .0; // filterbank channel bandwith
	int nchans = 0; // number of filterbank channels
	int nifs = 0; // number of seperate if channels
	double refdm = .0; // reference dispersion measure
	double period = .0; // folding period (s)

	int nbeams = 0;
	int ibeam = 0;
private:
	int hdrlen = 0;
	int filelen = 0;
	int nbytes = 0;
	std::string filename;

};

#endif // !FILTERBANK_HEADER_H

