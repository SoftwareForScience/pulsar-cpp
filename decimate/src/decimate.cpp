#include "decimate.h"

int main(int argc, char* argv[]) {
	
	std::vector<std::string> argList(argv, argv + argc);
	if (argList.size() == 1)
		std::cout << "Please supply a filterbank file \n";

	std::string filename = argList[1];

	int nsamp, naddc, naddt, headerless, obits;
	filterbank* input;
	filterbank* output;


	for (int i = 2; i < argList.size(); i++) {
		if (argList[i].compare("-c")) {
			i++;
			naddc = atoi(argv[i]);
		}
		else if (argList[i].compare("-t")) {
			i++;
			naddt = atoi(argv[i]);
		}
		else if (argList[i].compare("-o")) {
			/* get and open file for output */
			output = filterbank::create_filterbank(filename);
		}
		else if (argList[i].compare("-T")) {
			i++;
			nsamp = atoi(argv[i]);
		}
		else if (argList[i].compare("-n")) {
			i++;
			obits = atoi(argv[i]);
		}
		else if (argList[i].compare("-headerless")) {
			headerless = 1;
		}
		else if (asteria::file_exists(argList[i])) {
			input = filterbank::read_filterbank(filename);
		}
		else {
			//decimate_help();
			//sprintf(string, "unknown argument (%s) passed to decimate", argv[i]);
			//error_message(string);
		}
		i++;
	}
}
