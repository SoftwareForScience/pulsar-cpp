#include "decimate.h"

/**
 * reduces the amount of data by combining measurements from multiple samples 
 * and/or channels.
 * 
 * @param[in] argc the number of arguments provided to the program
 * @param[in] argv the arguments provided to the program
 */
int main(int argc, char* argv[]) {
	filterbank fb;
	CommandLineOptions opts;
	legacy_arguments(argc, argv, opts);
	CommandLineOptions::statusReturn_e argumentStatus = opts.parse(argc, argv);
	if (argumentStatus == CommandLineOptions::OPTS_SUCCESS) {
		//Casting int to enum filterbank::inputType
		fb = filterbank::read((filterbank::ioType)opts.getInputType(), opts.getInputFile());
		if (opts.getNumberOfBits()) {
			fb.header["nbits"].val.i = opts.getNumberOfBits();
		}

		if (opts.getNumberOfOutputSamples()) {
			decimate_samples(fb, (fb.header["nsamples"].val.i / opts.getNumberOfOutputSamples()));
		} else if (opts.getNumberOfSamples()){
			decimate_samples(fb, opts.getNumberOfSamples());
		} else {
			decimate_samples(fb, fb.header["nsamples"].val.i);
		}
		decimate_channels(fb, opts.getNumberOfChannels());
		fb.write((filterbank::ioType)opts.getOutputType(), opts.getOutputFile(), opts.getHeaderlessFlag());
	}
	else if (argumentStatus == CommandLineOptions::OPTS_HELP) {
		//Help printed
		exit(0);
	}
	else {
		exit(-1);
	}
}

/**
 * reduces the amount of data by combining measurements from multiple frequency channels
 * 
 * @param[in] fb Filterbank file to decimate
 * @param[in] n_channels_to_combine number of channels to combine into one measurement
 */
void decimate_channels(filterbank& fb, unsigned int n_channels_to_combine) {
	if (n_channels_to_combine < 1 || fb.header["nchans"].val.i % n_channels_to_combine) {
		std::cerr << "File does not contain a multiple of: " << n_channels_to_combine << " channels.\n";
		exit(-3);
	}
	
	unsigned int n_channels_out = fb.header["nchans"].val.i / n_channels_to_combine;
	unsigned int  n_values_out = fb.header["nifs"].val.i * n_channels_out * fb.header["nsamples"].val.i;

	std::vector<float> temp(n_values_out);

	for (unsigned int interface = 0; interface < fb.header["nifs"].val.i; interface++) {
		for (unsigned int sample = 0; sample < fb.header["nsamples"].val.i; sample++){
			unsigned int channel = 0;
			while (channel < fb.header["nchans"].val.i) {
				float total = 0;
				for (unsigned int j = 0; j < n_channels_to_combine; ++j) {
					unsigned int index = (sample * fb.header["nifs"].val.i * fb.header["nchans"].val.i) 
					+ (interface * fb.header["nchans"].val.i) 
					+ channel;

					total += fb.data[index];
					channel++;
				}
				float avg = total / n_channels_to_combine;

				unsigned int out_index = (sample * fb.header["nifs"].val.i * n_channels_out)
					+ (interface * n_channels_out)
					+ ((channel / n_channels_to_combine) - 1);
				temp[out_index] = avg;
			}
		}
	}

	fb.header["nchans"].val.i = n_channels_out;
	fb.data = temp;
}

/**
 * reduces the amount of data by combining measurements from multiple samples
 * 
 * @param[in] fb Filterbank file to decimate
 * @param[in] n_samples_to_combine number of samples to combine into one measurement
 */
void decimate_samples(filterbank& fb, unsigned int n_samples_to_combine) {
	if (n_samples_to_combine < 1 || fb.header["nsamples"].val.i % n_samples_to_combine) {
		std::cerr << "File does not contain a multiple of: " << n_samples_to_combine << " samples.\n";
		exit(-3);
	}

	unsigned int n_samples_out = fb.header["nsamples"].val.i / n_samples_to_combine;
	unsigned int n_values_out = fb.header["nifs"].val.i * fb.header["nchans"].val.i * n_samples_out;
	std::vector<float> temp(n_values_out);
    
	for(unsigned int channel =  0; channel < fb.header["nchans"].val.i; channel++){
		for(unsigned int interface = 0; interface < fb.header["nifs"].val.i; interface++){
			unsigned int sample = 0;
			while (sample < fb.header["nsamples"].val.i){
				float total = 0;
				for(unsigned int i= 0; i < n_samples_to_combine; i++){
					unsigned int index = (sample * fb.header["nifs"].val.i * fb.header["nchans"].val.i) 
					+ (interface * fb.header["nchans"].val.i)
					+ channel;
					total += fb.data[index];
					sample++;
				}		
				
				unsigned int out_index = ((((sample/n_samples_to_combine) -1) * fb.header["nifs"].val.i * fb.header["nchans"].val.i)
				+ (interface * fb.header["nchans"].val.i)
				+ channel);
				temp[out_index] = total;
			}
		}
	}

	fb.header["nsamples"].val.i = n_samples_out;

	// if we decrease the amount of samples, the time between samples increase
	fb.header["tsamp"].val.d = fb.header["tsamp"].val.d * n_samples_to_combine;
	fb.data = temp;
}

/**
 * Changes the -headerless parameter in the input arguments to --headerless
 * to allow boost programoptions to read the file
 * 
 * @param[in] argc the number of arguments provided to the program
 * @param[in] argv the arguments provided to the program
 * @param[in] opts a reference to the program options for this program
 */
void legacy_arguments(int argc, char* argv[], CommandLineOptions& opts) {
	//-headerless is not a valid switch, changing it to --headerless.
	int count = 0;
	std::vector<std::string> arguments(argv, argv + argc);
	for (std::string& s : arguments)
	{
		if (!s.compare("-headerless"))
		{
			argv[count] = (char*)"--headerless";
		}
		count++;
	}
}