#ifndef HEADERPARAM_H
#define HEADERPARAM_H

enum dataType
{
	INT,
	STRING,
	DOUBLE
};

union headerValue {
	int32_t i;
	char s[80]{"\0"};
	double d ;
};

struct header_param
{
	dataType type;
	headerValue val;

	header_param(){
		this->type = INT;
	}

	header_param(const dataType type) {
		this->type = type;
	}
};



#endif // !HEADERPARAM_H
