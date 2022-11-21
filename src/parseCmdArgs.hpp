/**
 * @file    parseCmdArgs.hpp
 * @brief   Functionality related to aligning mashmap mappings
 * @author  Chirag Jain <cjain7@gatech.edu>
 */


#include <iostream>
#include <string>
#include <fstream>


#include "./parameters.hpp"
#include "./argvparser.hpp"

namespace cli
{
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
$ junctions -w foo.msa -q bar.msa");

    cmd.setHelpOption("h", "help", "Print this help page");

    cmd.defineOption("w",
                     "a file containing an msa or ED-String in .eds format",
                     ArgvParser::OptionRequiresValue);

    cmd.defineOption("q",
                     "a file containing an msa or ED-String in .eds format",
                     ArgvParser::OptionRequiresValue);

    cmd.defineOption("verbosity",
                     "count of threads for parallel execution [default : 0]",
                     ArgvParser::OptionRequiresValue);
    cmd.defineOptionAlternative("verbosity", "v");

    cmd.defineOption("naive",
                     "use the naive implementation [disabled by default]");
    cmd.defineOptionAlternative("naive", "n");
  }

  /**
   * @brief                   Print the parsed cmd line options
   * @param[in]  parameters   parameters parsed from command line
   */
  void printCmdOptions(cli::Parameters &parameters)
  {
    std::cerr << "w = " << parameters.refSequences << std::endl;
    std::cerr << "q = " << parameters.querySequences << std::endl;
    std::cerr << std::endl;
    std::cerr << (parameters.naive ? "using naive implementation"
                                   : "using improved implementation")
              << std::endl;
    std::cerr << "verbosity = " << parameters.verbosity << std::endl;
  }

  /**
   * @brief                   Parse the cmd line options
   * @param[in]   cmd
   * @param[out]  parameters  alignment parameters are saved here
   */
  void parseandSave(int argc, char** argv, 
      CommandLineProcessing::ArgvParser &cmd, 
      cli::Parameters &parameters)
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

    //Parse reference files
    if(cmd.foundOption("w"))
    {
      std::string ref;

      str << cmd.optionValue("w");
      str >> ref;

      parameters.refSequences = ref;
    }
    str.clear();

    //Parse query files
    if(cmd.foundOption("q"))
    {
      std::string query;

      str << cmd.optionValue("q");
      str >> query;

      parameters.querySequences = query;
    }
    str.clear();

    if (cmd.foundOption("naive")) {
      parameters.naive = false;
    } else
      parameters.naive = true;

    if (cmd.foundOption("verbosity")) {
      str << cmd.optionValue("verbosity");
      str >> parameters.verbosity;
    } else
      parameters.verbosity = 0;

    printCmdOptions(parameters);
  }
}
