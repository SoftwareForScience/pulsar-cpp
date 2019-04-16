#include "decimate.h"

int main(int argc, char* argv[]) {
	
	std::vector<std::string> argList(argv, argv + argc);
	if (argList.size() == 1)
		std::cout << "Please supply a filterbank file \n";


	int nsamp, naddc, naddt, headerless, obits;
	std::string outputFile = "";
	filterbank input;

	for (int i = 1; i < argList.size(); i++) {
		if (!argList[i].compare("-c")) {
			i++;
			naddc = atoi(argv[i]);
		}
		else if (!argList[i].compare("-o")) {
			i++;
			outputFile = argList[i];
		}
		else if (!argList[i].compare("-T")) {
			i++;
			nsamp = atoi(argv[i]);
		}
		else if (asteria::file_exists(argList[i])) {
			std::string filename = argList[i];
			input = filterbank::read_filterbank(filename);

		}
		else {
			//decimate_help();
			//sprintf(string, "unknown argument (%s) passed to decimate", argv[i]);
			//error_message(string);
		}
	}
	decimate(input, outputFile, nsamp);
}

void decimate(filterbank& fb, std::string outputFileName, unsigned int n_samples_to_combine) {
	fb.filename = outputFileName;

	// Number of samples per channel
	unsigned int n_samples_c = fb.header["nsamples"].val.i / fb.n_channels;

	// total number of output samples 
	unsigned int n_samples_out = fb.header["nsamples"].val.i / n_samples_to_combine;
	unsigned int n_samples_c_out = n_samples_out / fb.n_channels;

	for (unsigned int channel = 0; channel < fb.n_channels; channel++) {
		for (unsigned int sample = 0; sample < n_samples_c_out; sample++) {
			float total = 0;
			for (int j = 0; j < n_samples_to_combine; ++j) {
				total += fb.data(channel, sample);
				sample++;
			}
			float avg = total / n_samples_to_combine;
			fb.data(channel, sample / n_samples_to_combine) = avg;
		}
	}
		
	fb.header["nsamples"].val.i = n_samples_out;
	fb.header["tsamp"].val.d = fb.header["tsamp"].val.d * n_samples_to_combine;

	//resize the matrix to our new format
	fb.data.resize(Eigen::NoChange, n_samples_c_out);
	fb.save_filterbank();
}
