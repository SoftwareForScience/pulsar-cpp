#pragma once

#include <iostream>
#include "filterbankBlock.h"

namespace Asteria {
	class OutputFile;
	class FilterbankBlock;

	class FilterbankFile {

	public:
		FilterbankFile(const char* filename);

		virtual void initialise();
		FilterbankBlock* readBlock(int start_sample, int len);

		int getChannelOffset(const FilterbankFile& other);

		// Did we hit the end of file?
		bool eof();		

		// is the filterbank file valid and readable?
		bool valid();			 

		const char* filename();

		double sample_interval();

	protected:
		char _source_name[80];

		int _machine_id, _telescope_id, _data_type, _nchans, _nbits, _nifs, _scan_number,
			_barycentric, _pulsarcentric;

		double _tstart, _mjdobs, _sample_interval, _fch1, _foff, _refdm, _az_start, _za_start, _src_raj, _src_dej;

		char _isign;

		bool _valid;
		int _header_length;

		const char* _filename;
		FILE* _rawfile;



		friend class OutputFilFile;
		friend class FilterbankBlock;

	};
}