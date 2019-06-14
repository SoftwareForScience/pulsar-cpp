#include "filterbankCore.hpp"

filterbank filterbank::read_stdio(std::string input) {
	std::string tempString;

	while (std::cin.good())
	{
		auto tempChar = std::cin.get();
		if (std::cin.good()) {
			tempString += tempChar;
		}
	}
}

void filterbank::write_stdio(bool headerless) {
	bool cmdOutput = false;
	if (outfilename.compare("") == 0) {
		std::FILE* tmpf = std::tmpfile();
		outf = tmpf;
		cmdOutput = true;
	}
	else {
		outf = fopen(outfilename.c_str(), "wb");
	}

	//TODO: Error handling
	if (outf == NULL) {
		std::cerr << "Failed to write to file \n";
		return;
	}

	if (save_header) {
		//TODO: Pointers? Rewrite to single method for any int/double/string?
		write_string("HEADER_START");
		for (auto param : header)
		{
			//Skip unused headers
			if (param.second.val.d == 0.0) {
				continue;
			}
			switch (param.second.type) {
			case INT: {
				write_value(param.first, param.second.val.i);
				break;
			}
			case DOUBLE: {
				write_value(param.first, param.second.val.d);
				break;
			}
			case STRING: {
				write_string(param.first);
				write_string(param.second.val.s);
				break;
			}
			}
		}
		write_string("HEADER_END");
	}

	for (uint32_t sample = 0; sample < n_samples; ++sample) {
		for (uint32_t interface = 0; interface < n_ifs; ++interface) {
			// Get the index for the interface
			int32_t index = (sample * n_ifs * n_channels) + (interface * n_channels);
			switch (n_bytes) {
			case 1: {
				std::vector<uint8_t>cwbuf(n_channels);
				for (uint32_t channel = start_channel; channel < end_channel; channel++) {
					cwbuf[channel - start_channel] = (uint8_t)data[((uint64_t)index) + channel];
				}

				fwrite(&cwbuf[0], sizeof(uint8_t), cwbuf.size(), outf);
				break;
			}
			case 2: {
				std::vector<uint16_t>swbuf(n_channels);
				for (uint32_t channel = start_channel; channel < end_channel; channel++) {
					swbuf[channel - start_channel] = (uint16_t)data[((uint64_t)index) + channel];
				}

				fwrite(&swbuf[0], sizeof(uint16_t), swbuf.size(), outf);

				break;
			}
			case 4: {
				fwrite(&data[index], sizeof(uint32_t), data.size(), outf);
				break;
			}
			}
		}
	}


	if (cmdOutput == true) {
		char s;
		std::rewind(outf);
		while ((s = fgetc(outf)) != EOF) {
			std::cout << s;
		}
	}

	fclose(inf);
	fclose(outf);
}
