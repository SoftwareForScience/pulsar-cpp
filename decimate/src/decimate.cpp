#include "decimate.h"

int main(int argc, char* argv[]) {

	std::vector<std::string> argList(argv, argv + argc);
	if (argList.size() == 1)
		std::cout << "Please supply a filterbank file \n";

	int naddc = 0, naddt = 0;
	std::string outputFile = "";
	filterbank fb;

	for (int i = 1; i < argList.size(); i++) {
		if (!argList[i].compare("-c")) {
			i++;
			naddc = atoi(argv[i]);
		}
		else if (!argList[i].compare("-o")) {
			i++;
			outputFile = argList[i];
		}
		else if (!argList[i].compare("-t")) {
			i++;
			naddt = atoi(argv[i]);
		}
		else if (asteria::file_exists(argList[i])) {
			std::string filename = argList[i];
			fb = filterbank::read_filterbank(filename);

		}
		else {
			//decimate_help();
			//sprintf(string, "unknown argument (%s) passed to decimate", argv[i]);
			//error_message(string);
		}
	}
	

	if(naddc)
		decimate_channels(fb, outputFile, naddc);
	if (naddt) {
		if (fb.n_channels * fb.header["nsamples"].val.i % naddt)
			std::cout << "File is not a multiple of " << naddt << "\n";
		decimate_samples(fb, outputFile, naddt);
	}
	fb.save_filterbank();
}

void decimate_channels(filterbank& fb, std::string outputFileName, unsigned int n_channels_to_combine) {
	fb.filename = outputFileName;

	unsigned int n_channels_out = fb.n_channels / n_channels_to_combine;

	Eigen::MatrixXd temp(fb.header["nsamples"].val.i, n_channels_out);

	for (int sample = 0; sample < fb.header["nsamples"].val.i; sample++) {
		for (int channel = 0; channel < fb.n_channels; channel++) {
			float total = 0;
			for (int j = 0; j < n_channels_to_combine; ++j) {
				total += fb.data(sample, channel);
				channel++;
			}
			float avg = total / n_channels_to_combine;
			unsigned int out_channel_index = (channel / n_channels_to_combine) - 1;
			temp(channel, out_channel_index) = avg;
		}
	}

	fb.header["nchans"].val.i = n_channels_out;
	//resize the matrix to our new format
	fb.data = temp;	
}

void decimate_samples(filterbank& fb, std::string outputFileName, unsigned int n_samples_to_combine) {
	fb.filename = outputFileName;

	unsigned int n_samples_out = fb.header["nsamples"].val.i / n_samples_to_combine;
	
	Eigen::MatrixXd temp(n_samples_out, fb.header["nchans"].val.i);

	for (int channel = 0; channel < fb.n_channels; channel++) {
		for (int sample = 0; sample < fb.header["nsamples"].val.i; sample++) {
			float total = 0;
			for (int j = 0; j < n_samples_to_combine; ++j) {
				total += fb.data(sample, channel);
				sample++;
			}

			float avg = total / n_samples_to_combine;
			unsigned int out_sample_index = (sample / n_samples_to_combine) - 1;
			temp(out_sample_index, channel) = avg;
		}
	}

	fb.header["nsamples"].val.i = n_samples_out;

	// if we decrease the amount of samples, the time between samples increase
	fb.header["tsamp"].val.d = fb.header["tsamp"].val.d * n_samples_to_combine;

	//resize the matrix to our new format
	fb.data = temp;
}