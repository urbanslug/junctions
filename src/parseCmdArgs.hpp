/**
 * @file    parseCmdArgs.hpp
 * @brief   Functionality related to aligning mashmap mappings
 * @author  Chirag Jain <cjain7@gatech.edu>
 */


#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <utility>

#include "./argvparser.hpp"
#include "./core.hpp"

namespace cli {
  // TODO: move this
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
              << "\nThis input should be an ED-string file in .eds format"
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
\
Provide at at least -i to check whether and intersection exists,\n\
or -g to compute the intersection graph\n\n\n\
\
Example usage: \n\
check whether the intersection exists\n\
$ ./build/junctions -i -a=2 data/x.eds data/y.eds\n\
compute the intersection graph\n\
$ ./build/junctions -g -d data/x.eds data/y.eds");

  cmd.setHelpOption("h", "help", "Print this help page");

  cmd.defineOption("info", "print info about the ed string and exit");
  cmd.defineOptionAlternative("info", "e");

  cmd.defineOption("intersect", "check whether the intersection is non-empty");
  cmd.defineOptionAlternative("intersect", "i");

  cmd.defineOption("algorithm", "(used with -i) algorithm to use:\n\
improved = 0\n\
naive = 1\n\
both = 2\n\
[default: 0]", ArgvParser::OptionRequiresValue);
  cmd.defineOptionAlternative("algorithm", "a");

  cmd.defineOption("graph", "compute the intersection graph");
  cmd.defineOptionAlternative("graph", "g");

  cmd.defineOption("print-dot",
      "output the intersection graph in dot format [default: false]");
  cmd.defineOptionAlternative("print-dot", "d");

  cmd.defineOption("multiset", "compute the size of the multiset");
  cmd.defineOptionAlternative("multiset", "m");

  cmd.defineOption("witness", "compute the shortest or longest witness\n\
shortest = short/shortest/0\n\
longest = short/longest/1",
                   ArgvParser::OptionRequiresValue);
  cmd.defineOptionAlternative("witness", "w");

  cmd.defineOption("match-stats-str", "matching stats ed string\n\
T_1 = 1\n\
T_2 = 2\n\
[default: 1]",
                   ArgvParser::OptionRequiresValue);
  cmd.defineOptionAlternative("match-stats-str", "T");

  cmd.defineOption("match-stats-idx", "matching stats letter index", ArgvParser::OptionRequiresValue);
  cmd.defineOptionAlternative("match-stats-idx", "n");

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

    if (parameters.task == core::arg::info ) {
      std::cerr << "Input files: ";
      for (auto f: parameters.input_files) {
        std::cerr << f.second << " (format: " << (f.first == 0 ? "msa" : "eds")
                  << ")" << std::endl;
      }
    
    } else {

    std::cerr << "w = " << parameters.w_file_path
              << " (format: "
              << (parameters.w_format == 0 ? "msa" : "eds")
              << ")" << std::endl;

    std::cerr << "q = " << parameters.q_file_path
              << " (format: "
              << (parameters.q_format == 0 ? "msa" : "eds")
              << ")" << std::endl;
    }

    switch (parameters.task) {
    case core::arg::compute_graph:
    std::cerr << "task: compute intersection graph." << std::endl;
    break;
    case core::arg::check_intersection:
    std::cerr << "task: check intersection" << std::endl;
    break;
    case core::arg::info:
    std::cerr << "task: print info" << std::endl;
    break;
    default:
    std::cerr << "Unhandled task: report a bug"  << std::endl;
    }

    if (parameters.output_dot) {
      std::cerr << "Print dot" << std::endl;
    }

    if (parameters.size_of_multiset) {
      std::cerr << "Compute the size of the multiset" << std::endl;
    }

    if (parameters.compute_witness) {
      std::cerr << "witness choice = " << parameters.witness_choice << std::endl;
    }

    if (parameters.compute_match_stats) {
      if (parameters.match_stats_str == 1 || parameters.match_stats_str == 2) {
        std::cerr << "match stats: str = " << parameters.match_stats_str
                  << " letter index = " << parameters.match_stats_letter_idx
                  << std::endl;
      }
    }

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
    } else if (!cmd.foundOption("e") && !cmd.foundOption("i") &&
               !cmd.foundOption("g")) {
        /*
          std::cerr << "Provide at at least -i to check whether and intersection
          " "exists,  or -g to compute the intersection graph"
                    << std::endl;
                    */

        std::cerr << cmd.usageDescription();
        exit(1);
    }

    std::stringstream str;

    /*
     * Info
     * ------------
     */

    if (cmd.foundOption("info")) {
      parameters.task = core::arg::info;
    }

    /*
     * Intersection
     * ------------
     */

    if (cmd.foundOption("intersect")) {
      parameters.task = core::arg::check_intersection;
    }

    // Algorithm
    // ---------
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

    /*
     * Graph
     * -----
     */

    // Compute Graph
    // -------------
    if (cmd.foundOption("graph")) {
      parameters.task = core::arg::compute_graph;
    } 

    // Matching stats
    if (cmd.foundOption("match-stats-str")) {
      parameters.compute_match_stats = true;
      str << cmd.optionValue("match-stats-str");

      if (str.str() == std::to_string(1)) {
        parameters.match_stats_str = 1;
      }

      if (str.str() == std::to_string(2)) {
        parameters.match_stats_str = 2;
      }
    } else
      parameters.match_stats_str = 0;
    str.clear();

    if (cmd.foundOption("match-stats-idx")) {
      parameters.compute_match_stats = true;
      str << cmd.optionValue("match-stats-idx");

      parameters.match_stats_letter_idx =  stoi(str.str());

    }
    str.clear();

    // Witness
    if (cmd.foundOption("witness")) {
      str << cmd.optionValue("witness");
      parameters.compute_witness = true;

      if (str.str() == "shortest" || str.str() == "short" || str.str() == std::to_string(0)) {
        parameters.witness_choice = core::witness::shortest;
      }

      if (str.str() == "longest" || str.str() == "long" || str.str() == std::to_string(1)) {
        parameters.witness_choice = core::witness::shortest;
      }
    } else
      parameters.compute_witness = false;
    str.clear();

    // Size of the multiset
    if (cmd.foundOption("multiset")) {
      parameters.size_of_multiset = true;
    } else {
      parameters.size_of_multiset = false;
    }

    // dot format
    if (cmd.foundOption("print-dot")) {
      parameters.output_dot = true;
    } else {
      parameters.output_dot = false;
    }

    /*
     * Other
     * -----
     */

    // Verbosity
    // ---------
    if (cmd.foundOption("verbosity")) {
      str << cmd.optionValue("verbosity");
      str >> parameters.verbosity;
    } else
      parameters.verbosity = 0;

    str.clear();

    /*
     * Args
     * ----
     */

    if (cmd.allArguments().empty()) {
      std::cerr << "Input files not provided." << std::endl;
      cmd.usageDescription();
      exit(1);
    }

    std::vector<string> args = cmd.allArguments();

    if (parameters.task == core::arg::info) {

      for (auto arg: args) {
        parameters.input_files.push_back(std::make_pair(extract_extension(arg), arg));
      }

    } else {
      // assume 2 input files
      parameters.w_format = extract_extension(args[0]);
      parameters.w_file_path = args[0];

      parameters.q_format = extract_extension(args[1]);
      parameters.q_file_path = args[1];
    }

    if (parameters.verbosity > 0) {
      printCmdOptions(parameters);
    }
  }
}
