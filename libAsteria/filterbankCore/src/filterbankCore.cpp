#include "filterbankCore.hpp"

std::map<uint16_t, std::string> filterbank::telescope_ids = {
	{ 0, "Fake" }, { 1, "Arecibo" }, { 1, "Ooty" },
	{ 3, "Nancay"}, { 4, "Parkes" },{ 5, "Jodrell"},
	{ 6, "GBT" }, { 7, "GMRT"},	{ 8, "Effelsberg"},
	{ 9, "ATA"}, { 10, "SRT"}, {11, "LOFAR"},
	{ 12, "VLA"}, { 20, "CHIME"}, {21, "FAST"},
	{64, "MeerKAT" }, {65, "KAT-7"}
};

std::map<uint16_t, std::string> filterbank::machine_ids = {
	{0,"FAKE"}, {1, "PSPM"}, {2, "WaPP"},
	{3, "AOFTM"}, {4,"BCPM1"}, {5, "OOTY"},
	{6, "SCAMP"}, {7, "SPIGOT"}, {11, "BG/P"},
	{12, "PDEV"}, {20, "CHIME+PSR"}, {64, "KAT"},
	{65, "KAT-DC2"}
};


filterbank filterbank::read(filterbank::ioType inType,
							std::string input) {

	filterbank fb;
	switch (inType) {
	case ioType::STDIO:
		fb = read_stdio(input);

	case ioType::FILEIO:
		fb = read_file(input);	
	}
}

void filterbank::write(filterbank::ioType outType,std::string filename, bool headerless) {
	//TODO: Error handling on IO
	switch (outType) {
	case ioType::STDIO:
		write_stdio(headerless);
	case ioType::FILEIO:
		write_file(filename, headerless);
	}
}

filterbank::filterbank() {
	// Make sure that data is allocated
	data = std::vector<float>(0);
}


