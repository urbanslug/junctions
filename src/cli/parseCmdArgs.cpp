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

#include "./cli.hpp"
#include "../core/core.hpp"
#include "../core/utils.hpp"


namespace cli {

/*
// TODO: move this to utils
core::file_format extract_extension(std::string const& file_path) {
  std::string extension = file_path.substr(file_path.find_last_of(".") + 1);
  core::file_format f;
  
  if (extension == "eds") {
    f = core::eds;
  }
  else if (extension == "msa") {
    f = core::msa;
  }
  else {
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
*/
  
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
$ ./bin/junctions -i -a=2 data/x.eds data/y.eds\n\
compute the intersection graph\n\
$ ./bin/junctions -g -d data/x.eds data/y.eds");

  cmd.setHelpOption("h", "help", "Print this help page");

  cmd.defineOption("info", "print info about the given ed string(s) and exit");
  cmd.defineOptionAlternative("info", "e");

  cmd.defineOption("intersect", "check whether the intersection is non-empty");
  cmd.defineOptionAlternative("intersect", "i");

  cmd.defineOption("algorithm", "(used with -i) algorithm to use:\n\
improved = 0\n\
naive = 1\n\
both = 2\n\
[default: 0]", CommandLineProcessing::ArgvParser::OptionRequiresValue);
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
                   CommandLineProcessing::ArgvParser::OptionRequiresValue);
  cmd.defineOptionAlternative("witness", "w");
  
  cmd.defineOption("match-stats-str",
                   "matching stats ed string\n" + core::T_1 + " = 1\n" +
                       core::T_2 + " = 2\n" + "[default: 1]",
                   CommandLineProcessing::ArgvParser::OptionRequiresValue);
  cmd.defineOptionAlternative("match-stats-str", "T");

  cmd.defineOption("match-stats-idx", "matching stats letter index",
                   CommandLineProcessing::ArgvParser::OptionRequiresValue);
  cmd.defineOptionAlternative("match-stats-idx", "n");

  cmd.defineOption("match-stats-avg",
				   "matching stats average");
  cmd.defineOptionAlternative("match-stats-avg", "s");

  cmd.defineOption("verbosity", "amount of debug information [default : 0]",
                   CommandLineProcessing::ArgvParser::OptionRequiresValue);
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
      return "unknown";
    };

    // core params

    // TODO: use method to check for the task
    if (parameters.get_task() == core::task::info ) {
      std::cerr << "Input files: ";
      for (auto f: parameters.input_files) {
        std::cerr << f.second << " (format: " << (f.first == 0 ? "msa" : "eds")
                  << ")" << std::endl;
      }
    }
	else {
	  std::cerr << "w = " << parameters.w_file_path
				<< " (format: "
				<< (parameters.w_format == 0 ? "msa" : "eds")
				<< ")" << std::endl;

	  std::cerr << "q = " << parameters.q_file_path
				<< " (format: "
				<< (parameters.q_format == 0 ? "msa" : "eds")
				<< ")" << std::endl;
    }

    std::cerr << "Task(s):\n";
    switch (parameters.get_task()) {
    case core::task::compute_graph:
	  std::cerr << core::indent(1) << "compute intersection graph\n";
	  break;
    case core::task::check_intersection:
	  std::cerr << core::indent(1) << "check intersection\n";
	  std::cerr << core::indent(1) << "algorithm = " << algo_string(parameters.algo) << "\n";
	  break;
    case core::task::info:
	  std::cerr << core::indent(1) << "print info\n";
	  break;
    default:
	  std::cerr << "unhandled task: report a bug\n";
    }

    if (parameters.output_dot) {
      std::cerr << core::indent(1) << "print dot\n";
    }

    if (parameters.size_of_multiset) {
      std::cerr << core::indent(1) << "compute the size of the multiset" << std::endl;
    }

    if (parameters.compute_witness()) {
      std::cerr << core::indent(1)
                << "witness choice = "
                << (parameters.witness_choice == core::witness::longest ? "longest" : "shortest")
                << "\n";
    }

    if (parameters.compute_match_stats) {
      if (parameters.match_stats_str == 1 || parameters.match_stats_str == 2) {
        std::cerr << core::indent(1)
                  << "match stats: str = " << parameters.match_stats_str
                  << " letter index = " << parameters.match_stats_letter_idx
                  << "\n";
      }
    }

    std::cerr << "verbosity = " << parameters.verbosity() << "\n";

    std::cerr << std::endl;
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
    if (result != CommandLineProcessing::ArgvParser::NoParserError)
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
      parameters.set_task(core::task::info);
    }

    /*
     * Intersection
     * ------------
     */

    if (cmd.foundOption("intersect")) {
      parameters.set_task(core::task::check_intersection);
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
      parameters.set_task(core::task::compute_graph);
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
    str.str("");
    str.clear();

    if (cmd.foundOption("match-stats-idx")) {
      parameters.compute_match_stats = true;
      str << cmd.optionValue("match-stats-idx");

      parameters.match_stats_letter_idx =  stoi(str.str());

    }
    str.str("");
    str.clear();

	if (cmd.foundOption("match-stats-avg")) {
	  parameters.compute_match_stats = true;
	  parameters.compute_match_stats_avg = true;
	}

    // Witness
    if (cmd.foundOption("witness")) {
      str << cmd.optionValue("witness");
      parameters.set_witness(true);

      if (str.str() == "shortest" || str.str() == "short" || str.str() == std::to_string(0)) {
        parameters.witness_choice = core::witness::shortest;
      }

      if (str.str() == "longest" || str.str() == "long" || str.str() == std::to_string(1)) {
        parameters.witness_choice = core::witness::longest;
      }
    } else
      parameters.set_witness(false);

    str.str("");
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
      unsigned char v;
      str >> v;
      parameters.set_verbosity(v);
    } else
      parameters.set_verbosity(0);

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

    std::vector<std::string> args = cmd.allArguments();

    if (parameters.is_task(core::task::info)) {

      for (auto arg: args) {
        parameters.input_files.push_back(std::make_pair(utils::extract_extension(arg), arg));
      }

    } else {
      // assume 2 input files
      parameters.w_format = utils::extract_extension(args[0]);
      parameters.w_file_path = args[0];

      parameters.q_format = utils::extract_extension(args[1]);
      parameters.q_file_path = args[1];
    }

    if (parameters.verbosity() > 0) {
      printCmdOptions(parameters);
    }
  }
}
