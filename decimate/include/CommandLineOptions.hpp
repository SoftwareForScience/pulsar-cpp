#ifndef _COMMAND_LINE_OPTIONS_HPP__
#define _COMMAND_LINE_OPTIONS_HPP__

#include <string.h>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>

namespace po = boost::program_options;

struct non_negative {
    uint32_t value;
};
void validate(boost::any& v, const std::vector<std::string>& values, non_negative*, int);

class CommandLineOptions {
public:
    enum statusReturn_e {
        OPTS_SUCCESS,
        OPTS_HELP,
        ERROR_IN_COMMAND_LINE,
        ERROR_UNHANDLED_EXCEPTION
    };
    CommandLineOptions();
    statusReturn_e parse(int argc, char* argv[]);

    const std::string & getInputFile() const;
    const std::string & getOutputFile() const;
    int32_t getNumberOfChannels() { return num_chans.value; };
    int32_t getNumberOfSamples() { return num_samps.value; };
    int32_t getNumberOfOutputSamples() { return num_output_samples.value; };
    int32_t getNumberOfBits() { return num_bits.value; };
    int getInputType() { return inputType; };
    int getOutputType() { return outputType; };
    bool getHeaderlessFlag() { return myHeaderlessFlag; };

protected:
    void setup();

private:
    po::options_description myOptions;
    po::positional_options_description myPositionalOptions;
    std::string myInputFile;
    std::string myOutputFile;
    int inputType;
    int outputType;
    non_negative num_chans;
    non_negative num_samps;
    non_negative num_output_samples;
    non_negative num_bits;
    bool myHeaderlessFlag;
};

inline
const std::string & CommandLineOptions::getInputFile() const {
    static const std::string emptyString;
    return (0 < myInputFile.length()) ? myInputFile : emptyString;
}
inline
const std::string & CommandLineOptions::getOutputFile() const {
    static const std::string emptyString;
    return (0 < myOutputFile.length()) ? myOutputFile : emptyString;
}


#endif // _COMMAND_LINE_OPTIONS_HPP__