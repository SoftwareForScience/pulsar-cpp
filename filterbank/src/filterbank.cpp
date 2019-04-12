#include "filterbank.h"


filterbank::filterbank(std::string filename) {

	this->filename = filename;
	ifs.open(this->filename, std::ifstream::in);

	if (!read_header(ifs)) {
		throw 1;
	}

	//header["telescope_id"]).val.s = "test";
}


bool filterbank::read_header(std::ifstream& f) {
	if (!f.is_open()) {
		return false;
	}

	unsigned int keylen = read_key_size(f);
	char* buffer = new char[keylen]{ '\0' };

	f.read(buffer, sizeof(char) * keylen);
	if (!strcmp(buffer, "HEADER_START")) {
		// if this isn't present, the file is not a valid filterbank file.
		return false;
	}

	while (true) {
		keylen = read_key_size(f);
		buffer = new char[keylen + 1]{ '\0' };
		f.read(buffer, sizeof(char) * keylen);

		if(!strcmp(buffer, "HEADER_END\0")) {
			break;
		}
		const std::string token(buffer);

		switch (header[token].type) {
		case INT: {
			header[token].val.i = read_int(f);
			break;
		}
		case DOUBLE: {
			header[token].val.d = read_double(f);
			break;
		}
		case STRING: {
			int len;
			auto value = read_string(f, &len);
			strncpy_s(header[token].val.s, value, len);
			break;
		}
		};

	}
	return true;
}


char* filterbank::read_string(std::ifstream& f, int* len) {
	unsigned int strlen;
	f.read(reinterpret_cast<char*>(&strlen), sizeof(strlen));
	char* buffer = new char[strlen + 1]{ '\0' };
	f.read(buffer, sizeof(char) * strlen);
	return buffer;
}

double filterbank::read_double(std::ifstream& f) {
	double val;
	f.read(reinterpret_cast<char*>(&val), sizeof(val));
	return val;
}

int filterbank::read_int(std::ifstream& f) {
	int value;
	f.read(reinterpret_cast<char*>(&value), sizeof(value));
	return value;
}

unsigned int filterbank::read_key_size(std::ifstream& f) {
	unsigned int keylen;
	f.read(reinterpret_cast<char*>(&keylen), sizeof(keylen));
	return keylen;
}