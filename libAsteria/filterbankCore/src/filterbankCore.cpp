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
 * @brief Construct a new filterbank::filterbank object
 * 
 */
filterbank::filterbank() {
	// Make sure that data is allocated
	data = std::vector<float>(0);
}

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
	FILE* fp = nullptr;
	switch (outType) {
		case ioType::STDIO: {
			fp = stdout;
			break;
		}
		case ioType::FILEIO: {
			auto fp = fopen(filename.c_str(), "wb");
			break;
		}
	}

	if (fp == NULL) {
		std::cerr << "Failed to open file for writing: " << filename << std::endl;
		return;
	}

	if (!headerless) {
		//Write the actual header
		write_string(fp, "HEADER_START");
		for (auto param : header) {
			//Skip unused headers
			if (param.second.val.d == 0.0) {
				continue;
			}
			switch (param.second.type) {
			case INT: {
				write_value(fp, param.first, param.second.val.i);
				break;
			}
			case DOUBLE: {
				write_value(fp, param.first, param.second.val.d);
				break;
			}
			case STRING: {
				write_string(fp, param.first);
				write_string(fp, param.second.val.s);
				break;
			}
			}
		}
		write_string(fp, "HEADER_END");
	}

	// Begin writing the data
	for (uint32_t sample = 0; sample < header["nsamples"].val.i; ++sample) {
		for (uint32_t interface = 0; interface < header["nifs"].val.i; ++interface) {
			// Get the index for the interface
			unsigned int index = (sample * header["nifs"].val.i * header["nchans"].val.i) + (interface * header["nchans"].val.i);
			switch (header["nbits"].val.i) {
				case 8: {
					std::vector<uint8_t>cwbuf(header["nchans"].val.i);
					for (unsigned int channel = 0; channel < header["nchans"].val.i; channel++) {
						// Check if data is bigger than the maximum size of uint8_t
						if(data[((uint64_t)index) + channel] > 0xff){
							cwbuf[channel] = 0xff;
						} else {
							cwbuf[channel] = (uint8_t)data[((uint64_t)index) + channel];
						}
					}

					fwrite(&cwbuf[0], sizeof(uint8_t), cwbuf.size(), fp);
					break;
				}
				case 16: {
					std::vector<uint16_t>swbuf(header["nchans"].val.i);
					for (unsigned int channel = 0; channel < header["nchans"].val.i; channel++) {
						// Check if the data is bigger than maximum size of uint16_t
						if(data[((uint64_t)index) + channel] > 0xffff){
							swbuf[channel] = 0xffff;
						} else {
							swbuf[channel] = (uint16_t)data[((uint64_t)index) + channel];
						}

					}

					fwrite(&swbuf[0], sizeof(uint16_t), swbuf.size(), fp);
					break;
				}
				case 32: {
					fwrite(&data[index], sizeof(uint32_t), data.size() / header["nsamples"].val.i, fp);
					break;
				}
				default:{
					std::cerr << "Invalid number of output bits: supported formats are 8/16/32 bits";
					break;
				}
			}
		}
	}
	if(fp != nullptr){
		fclose(fp);
	}
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