#include "header.h"

int main(int argc, char* argv[])
{
	std::vector<std::string> argList(argv, argv + argc);
	if (argList.size() == 1) 
		std::cout << "Please supply a filterbank file \n";
	
	std::string filename = argList[1];
	filterbank* fb = nullptr;

	//auto test = new filterbank_header(filename);
	try {
		fb = filterbank::read_filterbank(filename);
	}
	catch(const char* msg){
		std::cout << msg << "\n";
		exit(1);
	}


	std::cout << "Data file                        : " << filename << "\n";
	std::cout << "Header size (bytes)              : " << fb->header_size << "\n";
	if (fb->data_size)
		std::cout << "Data size (bytes)                : " << fb->data_size << "\n";
	if (fb->header["pulsarcentric"].val.i)
		std::cout << "Data type                        : " << fb->header["data_type"].val.i << "(pulsarcentric)\n";
	else if (fb->header["barycentric"].val.i)
		std::cout << "Data type                        : " << fb->header["data_type"].val.i << "(barycentric)\n";
	else
		std::cout << "Data type                        : " << fb->header["data_type"].val.i << "(topocentric)\n";

	std::cout << "Telescope                        : " << fb->telescope << "\n";
	std::cout << "Datataking Machine               : " << fb->backend << "\n";
	std::cout << "Source Name                      : " << fb->header["source_name"].val.s << "\n";


	// TODO: print this properly 
	/*if (fb->header["src_raj"].val.d)
		printf("Source RA (J2000)                : %02d:%02d:%s\n", rah, ram, sra);
	if (fb->header["src_raj"].val.d)
		printf("Source DEC (J2000)               : %c%02d:%02d:%s\n", decsign, abs(ded), dem, sde);*/

	if (fb->header["az_start"].val.d)
		std::cout << "Start AZ (deg)                   : " << fb->header["az_start"].val.d << "\n";
	if (fb->header["za_start"].val.d)
		std::cout << "Start ZA (deg)                   : " << fb->header["za_start"].val.d << "\n";

	switch (fb->header["data_type"].val.i) {
	case 0:
	case 1:
		if (!fb->header["fch1"].val.d && !fb->header["foff"].val.d) {
			std::cout << "Highest frequency channel (MHz)  : " << fb->frequencies.front() << "\n";
			std::cout << "Lowest frequency channel  (MHz)  : " << fb->frequencies.back() << "\n";
		}
		else {
			std::cout << "Frequency of channel 1 (MHz)     : " << fb->header["fch1"].val.d << "\n";
			std::cout << "Channel bandwidth      (MHz)     : " << abs(fb->header["foff"].val.d) << "\n";
			std::cout << "Number of channels               : " << fb->header["nchans"].val.i << "\n";
			std::cout << "Number of beams                  : " << fb->header["nbeams"].val.i << "\n";
			std::cout << "Beam number                      : " << fb->header["ibeam"].val.i << "\n";
		}
		break;
	case 2:
		std::cout << "Reference DM (pc/cc)             : " << fb->header["refdm"].val.i << "\n";
		std::cout << "Reference frequency    (MHz)     : " << fb->header["fch1"].val.i << "\n";
		break;
	case 3:
		std::cout << "Frequency of channel 1 (MHz)     : " << fb->header["fch1"].val.d << "\n";
		std::cout << "Channel bandwidth      (MHz)     : " << abs(fb->header["foff"].val.d) << "\n";
		std::cout << "Number of channels               : " << fb->header["nchans"].val.i << "\n";
		std::cout << "Number of beams                  : " << fb->header["nbeams"].val.i << "\n";
		std::cout << "Beam number                      : " << fb->header["ibeam"].val.i << "\n";
		break;
	case 6:
		std::cout << "Reference DM (pc/cc)             : " << fb->header["refdm"].val.i << "\n";
		std::cout << "Frequency of channel 1 (MHz)     : " << fb->header["fch1"].val.d << "\n";
		std::cout << "Channel bandwidth      (MHz)     : " << abs(fb->header["foff"].val.d) << "\n";
		std::cout << "Number of channels               : " << fb->header["nchans"].val.i << "\n";
		break;
	}

	std::cout << "Time stamp of first sample (MJD) : " << fb->header["tstart"].val.d << "\n";
	if (fb->header["data_type"].val.i != 3)
		std::cout << "Sample time (us)                 : " << fb->header["tsamp"].val.d * 1.0e6 << "\n";
	
	std::cout << "Number of bits per sample        : " << fb->header["nbits"].val.i << "\n";
	std::cout << "Number of IFs                    : " << fb->header["nifs"].val.i << "\n";

	return 0;
}