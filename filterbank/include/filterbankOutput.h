#pragma once
#include "filterbank.h"
#include "filterbankBlock.h"
#include <cmath>
#include <cstdlib>

namespace Asteria {
	class FilterbankOutput : public FilterbankFile {
	public:
		FilterbankOutput(const char* filename);
		FilterbankOutput(const char* filename, const FilterbankFile & copy);

		virtual void initialise();

		void writeBlock(const FilterbankBlock* block);
		FilterbankBlock* createBlock(int length);

		void setNbits(int nbits) {
			_nbits = nbits;
		}

		bool expandTo(const FilterbankFile& other)

	private:
		int _current_sample;
	};
}