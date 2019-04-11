#include "header.h"


inline void endian_swap(unsigned int& x)
{
	x = (x >> 24) |
		((x << 8) & 0x00FF0000) |
		((x >> 8) & 0x0000FF00) |
		(x << 24);
}

std::string read_string(std::ifstream & f) {
	unsigned int strlen;
	f.read(reinterpret_cast<char*>(&strlen), sizeof(strlen));
	char* buffer = new char[strlen + 1]{ '\0' };
	f.read(buffer, sizeof(char) * strlen);

	std::string str(buffer);
	return str;
}

double read_double(std::ifstream & f) {
	double val;
	f.read(reinterpret_cast<char*>(&val), sizeof(val));
	return val;
}

int read_int(std::ifstream & f) {
	int value;
	f.read(reinterpret_cast<char*>(&value), sizeof(value));
	return value;
}



filterbank_header::filterbank_header(std::string file)
{
	filename = file;

	//auto err = fopen_s(&fp, filename.c_str, "r");
	std::ifstream f;
	f.open(filename, std::ifstream::in);

	if (!f.is_open()) {
		throw 1;
	}

	unsigned int keylen;
	f.read(reinterpret_cast<char*>(&keylen), sizeof(keylen));

	char* buffer = new char[keylen + 1]{ '\0' };
	f.read(buffer, sizeof(char) * keylen);
	std::string token(buffer);

	if (strcmp(buffer, "HEADER_START"))
	{
		/* the data file is not in standard format, return */
		throw 2;
	}

	while (true) {
		f.read(reinterpret_cast<char*>(&keylen), sizeof(keylen));
		buffer = new char[keylen + 1]{ '\0' };
		f.read(buffer, sizeof(char) * keylen);

		if (!strcmp(buffer, "telescope_id")) {
			telescope_id = read_int(f);
		}
		else if (!strcmp(buffer, "machine_id")) {
			machine_id = read_int(f);
		}
		else if (!strcmp(buffer, "data_type")) {
			data_type = read_int(f);
		}
		else if (!strcmp(buffer, "rawdatafile")) {
			rawdatafile = read_string(f);
		}
		else if (!strcmp(buffer, "source_name")) {
			source_name = read_string(f);
		}
		else if (!strcmp(buffer, "barycentric")) {
			barycentric = read_int(f);
		}
		else if (!strcmp(buffer, "pulsarcentric")) {
			pulsarcentric = read_int(f);
		}
		else if (!strcmp(buffer, "az_start")) {
			az_start = read_double(f);
		}
		else if (!strcmp(buffer, "za_start")) {
			za_start = read_double(f);
		}
		else if (!strcmp(buffer, "src_raj")) {
			src_raj = read_double(f);
		}
		else if (!strcmp(buffer, "src_dej")) {
			src_dej = read_double(f);
		}
		else if (!strcmp(buffer, "tstart")) {
			tstart = read_double(f);
		}
		else if (!strcmp(buffer, "tsamp")) {
			tsamp = read_double(f);
		}
		else if (!strcmp(buffer, "nbits")) {
			nbits = read_int(f);
		}
		else if (!strcmp(buffer, "nsamples")) {
			nsamples = read_int(f);
		}
		else if (!strcmp(buffer, "fch1")) {
			fch1 = read_double(f);
		}
		else if (!strcmp(buffer, "foff")) {
			foff = read_double(f);
		}
		else if (!strcmp(buffer, "nchans")) {
			nchans = read_int(f);
		}
		else if (!strcmp(buffer, "nifs")) {
			nifs = read_int(f);
		}
		else if (!strcmp(buffer, "refdm")) {
			refdm = read_double(f);
		}
		else if (!strcmp(buffer, "period")) {
			period = read_double(f);
		}
		else if (!strcmp(buffer, "nbeams")) {
			nbeams = read_int(f);
		}
		else if (!strcmp(buffer, "ibeam")) {
			ibeam = read_int(f);
		}
		else if (!strcmp(buffer, "HEADER_END")) {
			break;
		}

	}

	//get the header length from the position in the stream.
	hdrlen = f.tellg();
	f.seekg(0, std::ios::ios_base::end);
	filelen = f.tellg();

	nbytes = filelen - hdrlen;
	nsamples = 8 * nbytes / nbits / nchans;
	f.close();
}

