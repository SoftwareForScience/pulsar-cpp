#pragma once

namespace Asteria {
	/* input and output files and logfile (filterbank.monitor) */
	FILE* input, * output, * logfile;
	char  inpfile[80], outfile[80];

	/* global variables describing the data */
	#include "filterbankHeader.h"
	double time_offset;

	/* global variables describing the operating mode */
	float start_time, final_time, clip_threshold;

	int obits, sumifs, headerless, headerfile, swapout, invert_band;
	int compute_spectra, do_vanvleck, hanning, hamming, zerolagdump;
	int headeronly;
	char ifstream[8];
	char flip_band;
	unsigned char* gmrtzap;
}