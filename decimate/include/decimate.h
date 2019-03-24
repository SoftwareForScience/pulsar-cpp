#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../../filterbank/include/filterbank.h"
#include "../../common/include/fileutils.h"


int nsamp, naddc, naddt, headerless, obits;
char inpfile[80], outfile[80];
FILE* input, * output, * logfile;
