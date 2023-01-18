/**
 * @file    parseCmdArgs.hpp
 * @brief   Functionality related to aligning mashmap mappings
 * @author  Chirag Jain <cjain7@gatech.edu>
 */


#include <iostream>
#include <string>
#include <fstream>

#include "./argvparser.hpp"
#include "./core.hpp"

namespace cli {
core::file_format extract_extension(std::string file_path) {
  auto const pos = file_path.find_last_of('.');
  core::file_format f;

  if (file_path.substr(pos + 1) == "eds") {
    f = core::eds;
  } else if (file_path.substr(pos + 1) == "msa") {
    f = core::msa;
  } else {
    f = core::unknown;
  }

  if (f == core::unknown) {
    std::cerr << "ERROR,Could not determine the format of "
              << file_path
              << "\nThis input should be an eds file or an MSA in..format "
              << std::endl;
    exit(1);
  }

  return f;
}

  /**
   * @brief           Initialize the command line argument parser
   * @param[out] cmd  command line parser object
   */
  void initCmdParser(CommandLineProcessing::ArgvParser &cmd)
  {
    cmd.setIntroductoryDescription("-----------------\n\
Compute the intersection of Elastic Degenerate Strings\n\
\n\
Example usage: \n\
$ ./build/junctions -a=2 -w data/x.eds -q data/y.eds");

    cmd.setHelpOption("h", "help", "Print this help page");

    cmd.defineOption(
                     "w",
                     "a file containing an msa (in RC-MSA format) or ED-String (in .eds format)",
                     ArgvParser::OptionRequiresValue);

    cmd.defineOption("q",
                     "a file containing an msa (in RC-MSA format) or ED-String "
                     "(in .eds format)",
                     ArgvParser::OptionRequiresValue);

    cmd.defineOption("algorithm", "algorithm to use:\n\
improved = 0\n\
naive = 1\n\
both = 2\n\
[default: 0]");
    cmd.defineOptionAlternative("algorithm", "a");

    cmd.defineOption("verbosity", "amount of debug information [default : 0]",
                     ArgvParser::OptionRequiresValue);
    cmd.defineOptionAlternative("verbosity", "v");
  }

  /**
   * @brief                   Print the parsed cmd line options
   * @param[in]  parameters   parameters parsed from command line
   */
  void printCmdOptions(core::Parameters &parameters) {

    auto algo_string =[](core::algorithm algo) -> std::string {
      switch (algo) {
      case core::algorithm::improved:
        return "improved";
      case core::algorithm::naive:
        return "naive";
      case core::algorithm::both:
        return "both";
      }
      return "uknown";
    };

    // core params
    std::cerr << "w = " << parameters.w_file_path
              << " (format: "
              << (parameters.w_format == 0 ? "msa" : "eds")
              << ")" << std::endl;

    std::cerr << "q = " << parameters.q_file_path
              << " (format: "
              << (parameters.q_format == 0 ? "msa" : "eds")
              << ")" << std::endl;

    // rest
    std::cerr << "algorithm = " << algo_string(parameters.algo) << std::endl;

    std::cerr << "verbosity = " << parameters.verbosity << std::endl;
  }

  /**
   * @brief                   Parse the cmd line options
   * @param[in]   cmd
   * @param[out]  parameters  alignment parameters are saved here
   */
  void parseandSave(int argc, char** argv,
                    CommandLineProcessing::ArgvParser &cmd,
                    core::Parameters &parameters)
  {
    int result = cmd.parse(argc, argv);

    //Make sure we get the right command line args
    if (result != ArgvParser::NoParserError)
      {
        std::cerr << cmd.parseErrorDescription(result) << std::endl;
        exit(1);
      }
    else if (!cmd.foundOption("w") )
      {
        std::cerr << "ERROR, align::parseandSave, Provide a w file\n\
        This input should be an eds file or an MSA in .. format" << std::endl;
        exit(1);
      }
    else if (!cmd.foundOption("q") )
      {
        std::cerr << "ERROR, align::parseandSave, Provide a w file\n\
        This input should be an eds file or an MSA in .. format"
                  << std::endl;
        exit(1);
      }

    std::stringstream str;

    // Parse w file
    if(cmd.foundOption("w"))
      {
        std::string ref;

        str << cmd.optionValue("w");
        str >> ref;

        parameters.w_format = extract_extension(ref);

        parameters.w_file_path = ref;
      }
    str.clear();

    // Parse query files
    if(cmd.foundOption("q"))
      {
        std::string query;

        str << cmd.optionValue("q");
        str >> query;

        parameters.q_format = extract_extension(query);

        parameters.q_file_path = query;
      }
    str.clear();

    // Algorithm
    //  -------
    if (cmd.foundOption("algorithm")) {
      str << cmd.optionValue("algorithm");

      std::string algo_str_arg;
      str >> algo_str_arg;

      if (algo_str_arg == "0") {
        parameters.algo = core::improved;
      } else if (algo_str_arg == "1") {
        parameters.algo = core::naive;
      } else if (algo_str_arg == "2") {
        parameters.algo = core::both;
      } else {
        parameters.algo = core::improved;
      }
    } else
      parameters.algo = core::improved;
    str.clear();


    // Verbosity
    //  -------
    if (cmd.foundOption("verbosity")) {
      str << cmd.optionValue("verbosity");
      str >> parameters.verbosity;
    } else
      parameters.verbosity = 0;

    if (parameters.verbosity > 0) {
      printCmdOptions(parameters);
    }
  }
}
