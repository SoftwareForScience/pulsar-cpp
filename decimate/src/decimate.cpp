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
			fb.read_data();
		}
		else {
			//decimate_help();
			//sprintf(string, "unknown argument (%s) passed to decimate", argv[i]);
			//error_message(string);
		}
	}
	
	fb.filename = outputFile;

	if(naddc)
		decimate_channels(fb, naddc);
	if (naddt) {
		if (fb.n_channels * fb.n_samples % naddt)
			std::cout << "File is not a multiple of " << naddt << "\n";
		decimate_samples(fb, naddt);
	}
	fb.save_filterbank();
}

void decimate_channels(filterbank& fb, unsigned int n_channels_to_combine) {

	unsigned int n_channels_out = fb.n_channels / n_channels_to_combine;
	unsigned int n_values_out = fb.n_ifs * n_channels_out * fb.n_samples;

	float * temp = new float[n_values_out];

	for (unsigned int sample = fb.start_sample; sample < fb.end_sample; sample++) {
		for (unsigned int interface = 0; interface < fb.n_ifs; interface++) {
			for (unsigned int channel = fb.start_channel; channel < fb.end_channel; channel++) {
				float total = 0;
				for (int j = 0; j < n_channels_to_combine; ++j) {
					int index = (sample * fb.n_ifs * fb.n_channels) + (interface * fb.n_channels) + channel;
					total += fb.data[index];
					channel++;
				}
				float avg = total / n_channels_to_combine;
				

				unsigned int out_index = (sample * fb.n_ifs * n_channels_out) + (interface * n_channels_out) + ((channel/n_channels_to_combine)) - 1;
				temp[out_index] = avg;
			}
		}
	}

	fb.header["nchans"].val.i = n_channels_out;
	fb.n_channels = n_channels_out;
	//resize the matrix to our new format
	fb.data = temp;	
}

void decimate_samples(filterbank& fb, unsigned int n_samples_to_combine) {
	unsigned int n_samples_out = fb.n_samples / n_samples_to_combine;
	unsigned int n_values_out = fb.n_ifs * fb.n_channels * n_samples_out;

	float* temp = new float[n_values_out];

	for (int channel = fb.start_channel; channel < fb.end_channel; channel++) {
		for (unsigned int interface = 0; interface < fb.n_ifs; interface++) {
			for (int sample = fb.start_sample; sample < fb.end_sample; sample++) {
				float total = 0;
				for (int j = 0; j < n_samples_to_combine; ++j) {
					unsigned int index = (sample * fb.n_ifs * fb.n_channels) + (interface * fb.n_channels) + channel;
					total += fb.data[index];
					sample++;
				}

				float avg = total / n_samples_to_combine;
				
				int out_index = (((sample/n_samples_to_combine) * fb.n_ifs * fb.n_channels) + (interface * fb.n_channels) + channel) -1;
				temp[out_index] = avg;
			}
		}
	}

	fb.header["nsamples"].val.i = n_samples_out;
	fb.n_samples = n_samples_out;

	// if we decrease the amount of samples, the time between samples increase
	fb.header["tsamp"].val.d = fb.header["tsamp"].val.d * n_samples_to_combine;

	//resize the matrix to our new format
	fb.data = temp;
}