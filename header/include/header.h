#ifndef HEADER_H
#define HEADER_H

#include <math.h>
#include <iostream>
#include <vector>
#include "filterbank-core.h"

class header {

    /* dmod(A,B) - A modulo B (double) */
    #define dmod(A,B) ((B)!=0.0?((A)*(B)>0.0?(A)-(B)*floor((A)/(B))\
                                        :(A)+(B)*floor(-(A)/(B))):(A))


/* dnint(A) - round to nearest whole number (double) */
#define dnint(A) ((A)<0.0?ceil((A)-0.5):floor((A)+0.5))

/* max(A,B) - larger (most +ve) of two numbers (generic) */
#define gmax(A,B) ((A)>(B)?(A):(B))


public:


private:

    void* getGregDate();
    void* julToGreg();


};

#endif // !HEADER_H
