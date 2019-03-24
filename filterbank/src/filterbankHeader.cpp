#include "filterbankHeader.h"

int nbins;
double period;

void get_string(FILE *inputfile, int *nbytes, char string[])
{
  int nchar;
  strcpy(string,"ERROR");
  fread(&nchar, sizeof(int), 1, inputfile);
  *nbytes=sizeof(int);
  if (feof(inputfile)) exit(0);
  if (nchar>80 || nchar<1) return;
  fread(string, nchar, 1, inputfile);
  string[nchar]='\0';
  *nbytes+=nchar;
}
