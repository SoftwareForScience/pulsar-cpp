#pragma once

#include <filterbank.h>
#include <stdint.h>
#include <cstdlib>

namespace Asteria {
	class FilterbankBlock {
	public:
		FilterbankBlock(uint64_t start, uint64_t length, const FilterbankFile* filfile);
		~FilterbankBlock();

		int read_block(FILE* input, int nbits, float* block, int nread);

		float* _data;

		const int _nchans;
		const int _nifs;

		const uint64_t _start;
		const uint64_t _length;
		const uint64_t _raw_length;
		const FilterbankFile* _filfile;
	private:
	};

} 
