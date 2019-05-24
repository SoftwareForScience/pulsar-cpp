#include "CommandLineOptions.hpp"

CommandLineOptions::CommandLineOptions():
    myOptions(""),
    myPositionalOptions(),
    myInputFile(""),
    myOutputFile(""),
    num_chans(1),
    num_samps(1),
    num_output_samples(1),
    myHeaderlessFlag(false)
{
    setup();
}

CommandLineOptions::~CommandLineOptions()
{

}

void CommandLineOptions::setup() {
    po::options_description options("decimate - reduce time and/or frequency resolution of filterbank data\n\n\
usage: decimate {filename} -{options}\n\noptions");
    options.add_options()
        ("help,h", "produce this help message")
        ("filename", po::value<std::string>(&myInputFile)->required()->value_name("FILE"), "filterbank data file (def=stdin)")
        (",o", po::value<std::string>(&myOutputFile)->value_name("FILE"), "filterbank output file (def=stdout)")
        (",c", po::value<int>(&num_chans)->default_value(1)->value_name("numchans"), "number of channels to add (def=all)")
        (",t", po::value<int>(&num_samps)->default_value(1)->value_name("numsamps"), "number of time samples to add (def=none)")
        (",T", po::value<int>(&num_output_samples)->default_value(1)->value_name("numsamps"), "(alternative to -t) specify number of output timesamples")
        (",n", po::value<int>(&num_bits)->value_name("numbits"), "specify output number of bits (def=input)")
        ("headerless", po::bool_switch(&myHeaderlessFlag)->default_value(false), "do not broadcast resulting header (def=broadcast)");

    myOptions.add(options);
    myPositionalOptions.add("filename", 1);
}

CommandLineOptions::statusReturn_e CommandLineOptions::parse(int argc, char* argv[]) {
    statusReturn_e ret = OPTS_SUCCESS;
    
    po::variables_map vm;
    std::string filename;

    try { 
        po::store(po::command_line_parser(argc, argv)
                    .options(myOptions)
                    .positional(myPositionalOptions).run(), 
                vm);

        if (vm.count("help")) { 
            std::cout << myOptions << std::endl;
            return OPTS_HELP; 
        }

        po::notify(vm);
    
    } catch (boost::program_options::required_option& ex_required) { 
        std::cerr << ex_required.what() << std::endl;
        return ERROR_IN_COMMAND_LINE; 
    } catch (const po::error &ex) {
        std::cerr << ex.what() << std::endl;
        return ERROR_IN_COMMAND_LINE; 
    }

    return ret;
}
