#include "decimate.h"

int main(int argc, char* argv[]) {
	
	std::vector<std::string> argList(argv, argv + argc);
	if (argList.size() == 1)
		std::cout << "Please supply a filterbank file \n";

	int nsamp = 0 , naddc;
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
	if (input.n_samples_c % nsamp != 0)
		std::cout << "File is not a multiple of " << nsamp << "\n";

	decimate(input, outputFile, nsamp);
}

void decimate(filterbank& fb, std::string outputFileName, unsigned int n_samples_to_combine) {
	fb.filename = outputFileName;

	// total number of output samples 
	int n_samples_out = (fb.n_channels * fb.n_samples_c) / n_samples_to_combine;
	int n_samples_c_out = n_samples_out / fb.n_channels;

	Eigen::MatrixXd temp(fb.n_channels, n_samples_c_out);

	for (int channel = 0; channel < fb.n_channels; channel++) {
		for (int sample = 0; sample < n_samples_c_out; sample++) {
			float total = 0;
			for (int j = 0; j < n_samples_to_combine; ++j) {
				total += fb.data(channel, sample);
				sample++;
			}
			float avg = total / n_samples_to_combine;
			int out_sample_index = (sample / n_samples_to_combine) - 1;
			temp(channel, out_sample_index) = avg;
		}
	}
		
	fb.header["nsamples"].val.i = n_samples_out;
	fb.n_samples_c = n_samples_c_out;

	fb.header["tsamp"].val.d = fb.header["tsamp"].val.d * n_samples_to_combine;

	//resize the matrix to our new format
	fb.data = temp;
	fb.save_filterbank();
}
