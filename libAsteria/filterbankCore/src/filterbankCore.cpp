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


filterbank filterbank::read(filterbank::ioType inType, std::string input = "") {

	filterbank fb;
	switch (inType) {
	case ioType::STDIO:
		fb = read_stdio(input);
	
	case ioType::FILEIO:
		fb = read_file(input);
	}

	//setup standard time/frequencies (take everything)
	fb.setup_time(false, false);
	fb.setup_frequencies(false, false);

}

void filterbank::write(filterbank::ioType outType,std::string filename = "", bool headerless = false) {
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

bool filterbank::read_data() {
	//TODO: Error handling
	// f = fopen(filename.c_str(), "rb");
	auto n_bytes_read = 0;

	if (inf == NULL) {
		return false;
	}

	// Allocate a block of data
	data = std::vector<float>(n_values);

	// Skip the header
	fseek(inf, header_size, SEEK_SET);

	// Skip the amount of samples we're offset from
	auto n_bytes_to_skip = (start_sample * n_ifs * header["nchans"].val.i);
	fseek(inf, n_bytes_to_skip, SEEK_CUR);

	for (uint32_t sample = 0; sample < n_samples; sample++) {
		for (uint32_t interface = 0; interface < n_ifs; interface++) {
			int32_t start_bytes_to_skip = start_channel * n_bytes;
			int32_t end_bytes_to_skip = (header["nchans"].val.i - end_channel) * n_bytes;

			//Skip the amount of channels we're not interested in
			fseek(inf, start_bytes_to_skip, SEEK_CUR);
			n_bytes_read += read_block(header["nbits"].val.i, &data[n_bytes_read], n_channels);
			//Skip the last few channels
			fseek(inf, end_bytes_to_skip, SEEK_CUR);
		}
	}

	// fclose(f);

	return true;
}

uint32_t filterbank::read_data(uint16_t nbits, float* block, const uint32_t nread) {
	size_t samples_read = 0;
	uint32_t sample = 0;
	std::vector<uint8_t> charblock(nread);
	std::vector<uint16_t> shortblock(nread);

	/* decide how to read the data based on the number of bits per sample */
	switch (nbits) {
	case 8: /* read n bytes into character block containing n 1-byte numbers */
		samples_read = fread(&charblock[0], sizeof(uint8_t), charblock.size(), inf);
		for (uint32_t i = 0; i < nread; i++) {
			block[i] = (float)charblock[i];
		}
		break;

	case 16: /* read 2*n bytes into short block containing n 2-byte numbers */
		samples_read = fread(&shortblock[0], sizeof(uint16_t), shortblock.size(), inf);
		for (uint32_t i = 0; i < samples_read; i++) {
			block[i] = (float)shortblock[i];
		}
		break;
	case 32:
		samples_read = fread(block, sizeof(float), nread, inf);
		break;
	}

	return (uint32_t)samples_read;

}

void filterbank::setup_frequencies(uint32_t startchan, uint32_t endchan) {
	double channel_bandwith = header["foff"].val.d;
	double start_frequency = header["fch1"].val.d;

	double f_start = startchan
		? ((startchan - start_frequency) / channel_bandwith)
		: 0;

	double f_stop = endchan
		? ((endchan - start_frequency) / channel_bandwith)
		: header["nchans"].val.i;


	for (uint32_t i = 0; i < n_channels; i++) {
		frequencies.push_back(i * channel_bandwith + start_frequency);
	}

	n_channels = int(f_stop - f_start);
	start_channel = (uint32_t)std::min(f_start, f_stop);
	end_channel = (uint32_t)std::max(f_start, f_stop);

	//recalculate number of values
	n_values = n_ifs * n_channels * n_samples;
}

void  filterbank::setup_time(uint32_t start, uint32_t end) {
	double sample_interval = header["tsamp"].val.d;
	double time_start = header["tstart"].val.d;

	if (start > end) {
		auto temp = end;
		end_sample = start;
		start_sample = end;
	}

	// if end_sample is not set (0) or it is bigger than the number of samples in the file
	// Set the number of samples accordingly
	if (!end_sample || end_sample > n_samples) {
		end_sample = n_samples;
	}

	n_samples = end_sample - start_sample;

	for (uint32_t i = 0; i < n_samples; i++) {
		timestamps.push_back(i * sample_interval / 24. / 60. / 60 + time_start);
	}

	// Recalculate number of values 
	n_values = n_ifs * n_channels * n_samples;

}

