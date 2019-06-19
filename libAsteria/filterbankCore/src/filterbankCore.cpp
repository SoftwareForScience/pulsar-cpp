#include "filterbankCore.hpp"

/**
 * @brief Contains a mapping of the known telescope id's and their names
 * 
 */
std::map<uint16_t, std::string> filterbank::telescope_ids = {
	{ 0, "Fake" }, { 1, "Arecibo" }, { 1, "Ooty" },
	{ 3, "Nancay"}, { 4, "Parkes" },{ 5, "Jodrell"},
	{ 6, "GBT" }, { 7, "GMRT"},	{ 8, "Effelsberg"},
	{ 9, "ATA"}, { 10, "SRT"}, {11, "LOFAR"},
	{ 12, "VLA"}, { 20, "CHIME"}, {21, "FAST"},
	{64, "MeerKAT" }, {65, "KAT-7"}
};

/**
 * @brief Contains a mapping of datataking machines and their names
 * 
 */
std::map<uint16_t, std::string> filterbank::machine_ids = {
	{0,"FAKE"}, {1, "PSPM"}, {2, "WaPP"},
	{3, "AOFTM"}, {4,"BCPM1"}, {5, "OOTY"},
	{6, "SCAMP"}, {7, "SPIGOT"}, {11, "BG/P"},
	{12, "PDEV"}, {20, "CHIME+PSR"}, {64, "KAT"},
	{65, "KAT-DC2"}
};

/**
 * @brief Reads a filterbank, either from standard io or a file
 * 
 * @param inType the input type, file or stdio
 * @param input the filename, or file as retrieved from stdio
 * @return filterbank the read filterbank file
 */
filterbank filterbank::read(filterbank::ioType inType, std::string input) {

	filterbank fb;
	switch (inType) {
		case ioType::STDIO:
			fb = read_stdio();
			break;
		case ioType::FILEIO:
			fb = read_file(input);
			break;
		}
	return fb;
}

/**
 * @brief Writes the current filterbank object to either a file or standard io
 * 
 * @param outType whether to write to stdio or a file
 * @param filename the filename to write, standard empty
 * @param headerless whether to pass the header
 */
void filterbank::write(filterbank::ioType outType, std::string filename, bool headerless) {
	//TODO: Error handling on IO
	switch (outType) {
		case ioType::STDIO: {
			write_stdio(headerless);
			break;
		}
		case ioType::FILEIO: {
			write_file(filename, headerless);
			break;
		}
	}
}

/**
 * @brief Construct a new filterbank::filterbank object
 * 
 */
filterbank::filterbank() {
	// Make sure that data is allocated
	data = std::vector<float>(0);
}

/**
 * @brief Helper method to write a string to a file.
 * this writes the length of the string first, then the actual data.
 */
void filterbank::write_string(FILE* fp, const std::string string) {
	uint32_t len = string.length();
	//Write the length of our string
	fwrite(&len, sizeof(int), 1, fp);
	//then write the actual string
	fwrite(string.c_str(), sizeof(char), len, fp);
	fflush(fp);
}