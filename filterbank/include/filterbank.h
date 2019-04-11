#ifndef FILTERBANK_H
#define FILTERBANK_H

#include <map>
#include <string>
#include <vector>
#include <header.h>

const std::map<uint16_t, std::string> telescope_ids = {
	{ 0, "Fake" }, { 1, "Arecibo" }, { 1, "Ooty" },
	{ 3, "Nancay"}, { 4, "Parkes" },{ 5, "Jodrell"},
	{ 6, "GBT" }, { 7, "GMRT"},	{ 8, "Effelsberg"}, 
	{ 9, "ATA"}, { 10, "SRT"}, {11, "LOFAR"},
	{ 12, "VLA"}, { 20, "CHIME"}, {21, "FAST"},
	{64, "MeerKAT" }, {65, "KAT-7"}
};

const std::map<uint16_t, std::string> machine_ids = {
	{0,"FAKE"}, {1, "PSPM"}, {2, "WaPP"},
	{3, "AOFTM"}, {4,"BCPM1"}, {5, "OOTY"},
	{6, "SCAMP"}, {7, "SPIGOT"}, {11, "BG/P"},
	{12, "PDEV"}, {20, "CHIME+PSR"}, {64, "KAT"},
	{65, "KAT-DC2"}
};


class filterbank {
public:
	filterbank_header header;

};

#endif // !FILTERBANK_H
