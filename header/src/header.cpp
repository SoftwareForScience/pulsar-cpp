#include "header.h"

double tobs;

void angle_split(double angle, int *dd, int *mm, double *ss) {
    int negative;
    if (angle < 0.0) {
        angle*= -1.0;
        negative = 1;
    } else {
        negative = 0;
    }
    *dd = (int) (angle / 10000.0);
    angle -= (double) (*dd) * 10000.0;
    *mm = (int) (angle / 100.0);
    *ss = angle - 100.0* (*mm);
    if (negative) *dd = *dd * -1;
}


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
	double ras, des;
	int rah, ram, ded, dem;
	char sra[6],sde[6], decsign;
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

	// Assign values to rah, ram and ras
    angle_split(fb.header["src_raj"].val.d,&rah,&ram,&ras);

    // Assign values to ded, dem and des
    angle_split(fb.header["src_dej"].val.d,&ded,&dem,&des);

    // Read tstart from the filterbank file
    double tstart = fb.header["tstart"].val.d;

	// Convert the Modified Julian Date to the gregorian date
    auto gregorian_date = boost::gregorian::gregorian_calendar::from_modjulian_day_number(tstart);

    // TODO: Refractor
    if (ras<10.0) {
        sprintf(sra,"0%.1f",ras);
    } else {
        sprintf(sra,"%.1f",ras);
    }

    // TODO: Refractor
    if (fb.header["src_dej"].val.d > 0.0)
        decsign = '+';
    else
        decsign = '-';
    if (des<10.0) {
        sprintf(sde,"0%.1f",des);
    } else {
        sprintf(sde,"%.1f",des);
    }

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

	angle_split(fb.header["src_raj"].val.d,&rah,&ram,&ras);
	if (ras > 10.0)
	    std::cout << "source RA (J2000)                : " << rah << ":" << ram << ":" << sra << std::endl;
/*    else
        std::cout << "ras = " << ras << std::endl;*/

	angle_split(fb.header["src_dej"].val.d,&ded,&dem,&des);
	if (fb.header["src_dej"].val.d > 0.0)
		std::cout << "Source DEC (J2000)               : " << decsign << abs(ded) << ":" << dem << ":" << sde
					<< std::endl;
/*    else
        std::cout << "src_dej = " << fb.header["src_dej"].val.d << std::endl;*/

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