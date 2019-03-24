#include "filterbankOutput.h"

namespace Asteria {
	FilterbankOutput::FilterbankOutput(const char* filename) : FilterbankFile(filename) {
	}

	FilterbankOutput::FilterbankOutput(const char* filename, const FilterbankFile& copy) : FilterbankFile(copy) {
		_filename = filename;
	}

	void FilterbankOutput::initialise()
	{
		for (int i = 0; i < _nifs; ++i) {
			ifstream[i] = 'Y';
		}
		
		obits = _nbits;
		_rawfile = fopen(_filename, "wb");
		filterbank_header(_rawfile);
	}

	FilterbankOutput::writeBlock(const FilterbankBlock* block) {
		if (_nbits != 32) {
			// TODO: Handle this
			return;
		}
		fwrite(block->_data, sizeof(float), block->_raw_length, _rawfile);
		break;

		FilterbankOutput::_current_sample += block->_length;
	}

	FilterbankBlock* FilterbankOutput::createBlock(int length)
	{
		return new FilterbankBlock::FilterbankBlock(_current_sample);
	}

	bool FilterbankBlock::FilterbankBlock(const FilterbankBlock& other) {
		if (other._foff != _foff) return false;
		if (other._tsamp != _tsamp) return false;
		if (other._tstart != _tstart) return false;
		if (other._foff * _foff < 0) return false;

		double start_chan = (other._fch1 - _fch1) / _foff;
		if (fabs(start_chan - round(start_chan)) > 0.01) {
			return false;
		}

		int istart_chan = (int)(round(start_chan));

		if (start_chan < 0) {
			// we need to start at an earlier channel.
			_fch1 += start_chan * _foff;
			_nchans -= start_chan;
		}


		double end_chan = (other._fch1 + other._nchans * other._foff - _fch1) / _foff;
		if (fabs(end_chan - round(end_chan)) > 0.01) {
			return false;
		}

		int iend_chan = (int)(round(end_chan));
		if (iend_chan > _nchans) {
			_nchans = iend_chan;
		}

		return true;
	}

}