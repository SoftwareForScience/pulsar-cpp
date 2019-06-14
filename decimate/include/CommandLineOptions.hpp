#ifndef _COMMAND_LINE_OPTIONS_HPP__
#define _COMMAND_LINE_OPTIONS_HPP__

#include <string.h>
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

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
    int32_t getNumberOfChannels() { return num_chans; };
    int32_t getNumberOfSamples() { return num_samps; };
    int32_t getNumberOfOutputSamples() { return num_output_samples; };
    int32_t getNumberOfBits() { return num_bits; };
    int getInputType() { return inputType; };
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
    int num_chans;
    int num_samps;
    int num_output_samples;
    int num_bits;
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