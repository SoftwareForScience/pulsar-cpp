#include "header.h"


void julToGreg ( int ndp, double djm, int iymdf[4], int *j ) /*includefile*/
/*
**  - - - - - - - - -
**   s l a D j c a l
**  - - - - - - - - -
**
**  Modified Julian Date to Gregorian calendar, expressed
**  in a form convenient for formatting messages (namely
**  rounded to a specified precision, and with the fields
**  stored in a single array).
**
**  Given:
**     ndp      int       number of decimal places of days in fraction
**     djm      double    Modified Julian Date (JD-2400000.5)
**
**  Returned:
**     iymdf    int[4]    year, month, day, fraction in Gregorian calendar
**     *j       int       status:  nonzero = out of range
**
**  Any date after 4701BC March 1 is accepted.
**
**  Large ndp values risk internal overflows.  It is typically safe
**  to use up to ndp=4.
**
**  The algorithm is derived from that of Hatcher 1984 (QJRAS 25, 53-55).
**
**  Defined in slamac.h:  dmod
**
**  Last revision:   17 August 1999
**
**  Copyright P.T.Wallace.  All rights reserved.
*/
{
    double fd, df, f, d;
    long jd, n4, nd10;

/* Validate */
    if ( ( djm <= -2395520.0 ) || ( djm >= 1.0e9 ) ) {
        *j = - 1;
        return;
    } else {

        /* Denominator of fraction */
        fd = pow ( 10.0, (double) std::max(ndp, 0));
        fd = std::round(fd);

        /* Round date and express in units of fraction */
        df = djm * fd;
        df = std::round(df);



        /* Separate day and fraction */
        f = dmod ( df, fd );
        if ( f < 0.0 ) f += fd;
        d = ( df - f ) / fd;

        /* Express day in Gregorian calendar */
        jd = (long) std::round(d) + 2400001L;
        n4 = 4L * ( jd + ( ( 2L * ( ( 4L * jd - 17918L ) / 146097L)
                             * 3L ) / 4L + 1L ) / 2L - 37L );
        nd10 = 10L * ( ( ( n4 - 237L ) % 1461L ) / 4L ) + 5L;
        iymdf[0] = (int) ( ( n4 / 1461L ) - 4712L );
        iymdf[1] = (int) ( ( ( nd10 / 306L + 2L ) % 12L ) + 1L );
        iymdf[2] = (int) ( ( nd10 % 306L ) / 10L + 1L );
        iymdf[3] = (int) std::round(f);
        *j = 0;
    }
}

void slaDjcal ( int ndp, double djm, int iymdf[4], int *j ) /*includefile*/
/*
**  - - - - - - - - -
**   s l a D j c a l
**  - - - - - - - - -
**
**  Modified Julian Date to Gregorian calendar, expressed
**  in a form convenient for formatting messages (namely
**  rounded to a specified precision, and with the fields
**  stored in a single array).
**
**  Given:
**     ndp      int       number of decimal places of days in fraction
**     djm      double    Modified Julian Date (JD-2400000.5)
**
**  Returned:
**     iymdf    int[4]    year, month, day, fraction in Gregorian calendar
**     *j       int       status:  nonzero = out of range
**
**  Any date after 4701BC March 1 is accepted.
**
**  Large ndp values risk internal overflows.  It is typically safe
**  to use up to ndp=4.
**
**  The algorithm is derived from that of Hatcher 1984 (QJRAS 25, 53-55).
**
**  Defined in slamac.h:  dmod
**
**  Last revision:   17 August 1999
**
**  Copyright P.T.Wallace.  All rights reserved.
*/
{
	double fd, df, f, d;
	long jd, n4, nd10;

/* Validate */
	if ( ( djm <= -2395520.0 ) || ( djm >= 1.0e9 ) ) {
		*j = - 1;
		return;
	} else {

		/* Denominator of fraction */
		fd = pow ( 10.0, (double) gmax ( ndp, 0 ) );
		fd = dnint ( fd );

		/* Round date and express in units of fraction */
		df = djm * fd;
		df = dnint ( df );

		/* Separate day and fraction */
		f = dmod ( df, fd );
		if ( f < 0.0 ) f += fd;
		d = ( df - f ) / fd;

		/* Express day in Gregorian calendar */
		jd = (long) dnint ( d ) + 2400001L;
		n4 = 4L * ( jd + ( ( 2L * ( ( 4L * jd - 17918L ) / 146097L)
							 * 3L ) / 4L + 1L ) / 2L - 37L );
		nd10 = 10L * ( ( ( n4 - 237L ) % 1461L ) / 4L ) + 5L;
		iymdf[0] = (int) ( ( n4 / 1461L ) - 4712L );
		iymdf[1] = (int) ( ( ( nd10 / 306L + 2L ) % 12L ) + 1L );
		iymdf[2] = (int) ( ( nd10 % 306L ) / 10L + 1L );
		iymdf[3] = (int) dnint ( f );
		*j = 0;
	}
}

void getGregDate(double djm, int *year, int *month, int *day) /*includefile*/
{
    int iymdf[4],j;
    char message[80];
	slaDjcal(1,djm,iymdf,&j);
    if (j==0) {
        *year=iymdf[0];
        *month=iymdf[1];
        *day=iymdf[2];
        return;
    } else {
        printf("julToGreg could not process MJD: %f",djm);
    }
}

int main(int argc, char* argv[]){

	static double tstart;
	int i,j,year,month,day,check,rah,ram,ded,dem;


	std::vector<std::string> argList(argv, argv + argc);
	if (argList.size() == 1) 
		std::cout << "Please supply a filterbank-core file \n";
	
	std::string filename = argList[1];
	filterbank fb;

	//auto test = new filterbank_header(filename);
	try {
		fb = filterbank::read_filterbank(filename);
	}
	catch(const char* msg){
		std::cout << msg << "\n";
		exit(1);
	}


	getGregDate(tstart,&year,&month,&day);
	

	std::cout << "Data file                        : " << filename << "\n";
	std::cout << "Header size (bytes)              : " << fb.header_size << "\n";
	if (fb.data_size)
		std::cout << "Data size (bytes)                : " << fb.data_size << "\n";
	if (fb.header["pulsarcentric"].val.i)
		std::cout << "Data type                        : " << fb.header["data_type"].val.i << "(pulsarcentric)\n";
	else if (fb.header["barycentric"].val.i)
		std::cout << "Data type                        : " << fb.header["data_type"].val.i << "(barycentric)\n";
	else
		std::cout << "Data type                        : " << fb.header["data_type"].val.i << "(topocentric)\n";

	std::cout << "Telescope                        : " << fb.telescope << "\n";
	std::cout << "Datataking Machine               : " << fb.backend << "\n";
	std::cout << "Source Name                      : " << fb.header["source_name"].val.s << "\n";


	// TODO: print this properly


//	if (fb.header["src_raj"].val.d)
//		printf("Source RA (J2000)                : %02d:%02d:%s\n", rah, ram, sra);
//	if (fb.header["src_raj"].val.d)
//		printf("Source DEC (J2000)               : %c%02d:%02d:%s\n", decsign, abs(ded), dem, sde);

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
		break;
	}

	std::cout << "Time stamp of first sample (MJD) : " << fb.header["tstart"].val.d << "\n";
    printf("Gregorian date (YYYY/MM/DD)      : %4d/%02d/%02d\n",year,month,day);
	if (fb.header["data_type"].val.i != 3)
		std::cout << "Sample time (us)                 : " << fb.header["tsamp"].val.d * 1.0e6 << "\n";

//	if (datasize && data_type != 3) {
//		printf("Number of samples                : %lld\n",numsamps);
//		tobs=(double)numsamps*tsamp;
//		strcpy(unit,"(seconds)   ");
//		if (tobs>60.0) {
//			tobs/=60.0;
//			strcpy(unit,"(minutes)   ");
//			if (tobs>60.0) {
//				tobs/=60.0;
//				strcpy(unit,"(hours)     ");
//				if (tobs>24.0) {
//					tobs/=24.0;
//					strcpy(unit,"(days)      ");
//				}
//			}
//		}
//		printf("Observation length %s  : %.1f\n",unit,tobs);
//	}

	std::cout << "Number of bits per sample        : " << fb.header["nbits"].val.i << "\n";
	std::cout << "Number of IFs                    : " << fb.header["nifs"].val.i << "\n";

	return 0;
}