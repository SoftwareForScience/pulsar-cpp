#ifndef FILTERBANK_HEADER_PARAM_H

#include <string>

class filterbank_header_param {
public:
	std::string key;
	enum {STRING, INT, DOUBLE, CHAR} datatype;
	union {char[80] s, int i, double d, char c } value;
};
#endif // !FILTERBANK_HEADER_PARAM_H

