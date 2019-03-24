#include "filterbankFile.h"

Asteria::FilterbankFile::FilterbankFile(const char* filename) {
	// assume file is invalid untill opened
	_valid = false;
	_filename = filename;
}

bool Asteria::FilterbankFile::valid() { return _valid; }

const char* Asteria::FilterbankFile::filename() { return _filename; }

double Asteria::FilterbankFile::sample_interval() { return _sample_interval; }

int Asteria::FilterbankFile::getChannelOffset(const FilterbankFile& other) {
	double start_channel = (other._fch1 - _fch1) / _foff;
	return((int)(round(start_channel)));
}

Asteria::FilterbankBlock* Asteria::FilterbankFile::readBlock(int start_sample, int length) {
	FilterbankBlock* block = new FilterbankBlock(start_sample, length, this);
	block->read_block(_rawfile, _nbits, block->_data, length * _nchans * _nifs);
	return block;
}

bool Asteria::FilterbankFile::eof()	{ return feof(_rawfile); }
