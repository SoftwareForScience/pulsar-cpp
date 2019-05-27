#include "header.h"

double tobs;

int get_obs_unit() {

    int i = 0;
    if (tobs > 60.0) {
        tobs/=60.0;
        i++;
        if (tobs > 60.0) {
            tobs /= 60.0;
            i++;
            if (tobs > 24) {
                tobs /= 24;
                i++;
            }
        }
    }
    return i;

}

int main(int argc, char* argv[]){

    int index;
	std::string unit[4] = {"(seconds)    ", "(minutes)    ", "(hours)      ", "(days)      "};

	std::vector<std::string> argList(argv, argv + argc);
	if (argList.size() == 1) 
		std::cout << "Please supply a filterbank-core file \n";
	
	std::string filename = argList[1];
	filterbank fb;

	try {
		fb = filterbank::read_filterbank(filename);
	}
	catch(const char* msg){
		std::cout << msg << "\n";
		exit(1);
	}

    // Read tstart from the filterbank file
    double tstart = fb.header["tstart"].val.d;

	// Convert the Modified Julian Date to the gregorian date
    auto gregorian_date = boost::gregorian::gregorian_calendar::from_modjulian_day_number(tstart);

	std::cout << "Data file                        : " << filename << "\n";
	std::cout << "Header size (bytes)              : " << fb.header_size << "\n";
	if (fb.data_size)
		std::cout << "Data size (bytes)                : " << fb.data_size << "\n";
	if (fb.header["pulsarcentric"].val.i)
		std::cout << "Data type                        : " << fb.header["data_type"].val.i << "(pulsarcentric)\n";
	else if (fb.header["barycentric"].val.i)
		std::cout << "Data type                        : " << fb.header["data_type"].val.i << "(barycentric)\n";
	else {
	    if (fb.header["data_type"].val.i == 1) {
            std::cout << "Data type                        : " << "filterbank (topocentric)\n";
	    } else
	        std::cout << "Data type                        : " << fb.header["data_type"].val.i << "(topocentric)\n";
	}

	std::cout << "Telescope                        : " << fb.telescope << "\n";
	std::cout << "Datataking Machine               : " << fb.backend << "\n";
	std::cout << "Source Name                      : " << fb.header["source_name"].val.s << "\n";


	// TODO: print this properly

//	if (fb.header["src_raj"].val.d)
//	    std::cout << "source RA (J2000)" <<
////		printf("Source RA (J2000)                : %02d:%02d:%s\n", rah, ram, sra);
//	if (fb.header["src_raj"].val.d)
////		printf("Source DEC (J2000)               : %c%02d:%02d:%s\n", decsign, abs(ded), dem, sde);

	if (fb.header["az_start"].val.d)
		std::cout << "Start AZ (deg)                   : " << fb.header["az_start"].val.d << "\n";
	if (fb.header["za_start"].val.d)
		std::cout << "Start ZA (deg)                   : " << fb.header["za_start"].val.d << "\n";

	switch (fb.header["data_type"].val.i) {
	case 0:
	case 1:
		if (!fb.header["fch1"].val.d && !fb.header["foff"].val.d) {
			std::cout << "Highest frequency channel (MHz)  : " << fb.frequencies.front() << "\n";
			std::cout << "Lowest frequency channel  (MHz)  : " << fb.frequencies.back() << "\n";
		}
		else {
			std::cout << "Frequency of channel 1 (MHz)     : " << fb.header["fch1"].val.d << "\n";
			std::cout << "Channel bandwidth      (MHz)     : " << abs(fb.header["foff"].val.d) << "\n";
			std::cout << "Number of channels               : " << fb.header["nchans"].val.i << "\n";
			std::cout << "Number of beams                  : " << fb.header["nbeams"].val.i << "\n";
			std::cout << "Beam number                      : " << fb.header["ibeam"].val.i << "\n";
		}
		break;
	case 2:
		std::cout << "Reference DM (pc/cc)             : " << fb.header["refdm"].val.i << "\n";
		std::cout << "Reference frequency    (MHz)     : " << fb.header["fch1"].val.i << "\n";
		break;
	case 3:
		std::cout << "Frequency of channel 1 (MHz)     : " << fb.header["fch1"].val.d << "\n";
		std::cout << "Channel bandwidth      (MHz)     : " << abs(fb.header["foff"].val.d) << "\n";
		std::cout << "Number of channels               : " << fb.header["nchans"].val.i << "\n";
		std::cout << "Number of beams                  : " << fb.header["nbeams"].val.i << "\n";
		std::cout << "Beam number                      : " << fb.header["ibeam"].val.i << "\n";
		break;
	case 6:
		std::cout << "Reference DM (pc/cc)             : " << fb.header["refdm"].val.i << "\n";
		std::cout << "Frequency of channel 1 (MHz)     : " << fb.header["fch1"].val.d << "\n";
		std::cout << "Channel bandwidth      (MHz)     : " << abs(fb.header["foff"].val.d) << "\n";
		std::cout << "Number of channels               : " << fb.header["nchans"].val.i << "\n";
		std::cout << "Number of channels               : " << fb.header["nchans"].val.i << "\n";
		break;
	}

	std::cout << "Time stamp of first sample (MJD) : " << fb.header["tstart"].val.d << "\n";
	std::cout << "Gregorian date (YYYY/MM/DD)      : " << gregorian_date.year << "/" << gregorian_date.month
	            << "/" << gregorian_date.day << std::endl;

	if (fb.header["data_type"].val.i != 3)
		std::cout << "Sample time (us)                 : " << fb.header["tsamp"].val.d * 1.0e6 << "\n";

	if (fb.data_size && fb.header["data_type"].val.i != 3) {
	    std::cout << "Number of samples                : " << fb.header["nsamples"].val.i << std::endl;

	    tobs = (double) fb.header["nsamples"].val.i * fb.header["tsamp"].val.d;

	    index = get_obs_unit();

	    std::cout << "Observation length " << unit[index] << " : " << tobs << std::endl;
    }

	std::cout << "Number of bits per sample        : " << fb.header["nbits"].val.i << "\n";
	std::cout << "Number of IFs                    : " << fb.header["nifs"].val.i << "\n";

	return 0;
}