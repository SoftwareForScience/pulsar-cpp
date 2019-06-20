#include "CommandLineOptions.hpp"

CommandLineOptions::CommandLineOptions():
    myOptions(),
    myPositionalOptions(),
    inputType(0),
    outputType(0),
    myInputFile(),
    myOutputFile(),
    num_chans(),
    num_samps(),
    num_output_samples(),
    num_bits(),
    myHeaderlessFlag(false)
{
    setup();
}

void CommandLineOptions::setup() {
    po::options_description options("decimate - reduce time and/or frequency resolution of filterbank data\n\n\
usage: decimate {filename} -{options}\n\noptions");
    options.add_options()
        ("help,h", "produce this help message")
        ("filename", po::value<std::string>(&myInputFile)->value_name("FILE"), "filterbank data file (def=stdin)")
        (",o", po::value<std::string>(&myOutputFile)->value_name("FILE"), "filterbank output file (def=stdout)")
        (",c", po::value<non_negative>(&num_chans)->value_name("numchans"), "number of channels to add (def=all)")
        (",t", po::value<non_negative>(&num_samps)->value_name("numsamps"), "number of time samples to add (def=none)")
        (",T", po::value<non_negative>(&num_output_samples)->value_name("numsamps"), "(alternative to -t) specify number of output timesamples")
        (",n", po::value<non_negative>(&num_bits)->value_name("numbits"), "specify output number of bits (def=input)")
        ("headerless", po::bool_switch(&myHeaderlessFlag), "do not broadcast resulting header (def=broadcast)");

    myOptions.add(options);
    myPositionalOptions.add("filename", 1);
}

CommandLineOptions::statusReturn_e CommandLineOptions::parse(int argc, char* argv[]) {
    statusReturn_e ret = OPTS_SUCCESS;
    
    po::variables_map vm;

    try { 
        po::store(po::command_line_parser(argc, argv)
                    .options(myOptions)
                    .positional(myPositionalOptions).run(), 
                vm);

        if (vm.count("help")) { 
            std::cout << myOptions << std::endl;
            return OPTS_HELP; 
        }
        if (vm.count("filename")) {
            inputType = 1;
        }
        if (vm.count("-o")) {
            outputType = 1;
        }

        po::notify(vm);
    
    } catch (boost::program_options::required_option& ex_required) { 
        std::cerr << ex_required.what() << std::endl;
        std::cout << myOptions << std::endl;
        return ERROR_IN_COMMAND_LINE; 
    } catch (const po::error &ex) {
        std::cerr << ex.what() << std::endl;
        std::cout << myOptions << std::endl;
        return ERROR_IN_COMMAND_LINE; 
    }

    return ret;
}

void validate(boost::any& v, const std::vector<std::string>& values, non_negative*, int) {
    po::validators::check_first_occurrence(v);

    std::string const& s = po::validators::get_single_string(values);
    if (s[0] == '-') {
        throw po::validation_error(po::validation_error::invalid_option_value);
    }
    v = boost::any(non_negative { boost::lexical_cast<uint32_t>(s) } );
}