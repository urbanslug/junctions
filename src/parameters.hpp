/**
 * @file    align_parameters.hpp
 * @author  Chirag Jain <cjain7@gatech.edu>
 */

#ifndef ALIGN_PARAMETERS_HPP
#define ALIGN_PARAMETERS_HPP

#include <string>
#include <vector>

namespace cli {
  /**
   * @brief   parameters for generating mashmap alignments
   */

struct Parameters {
  bool naive;
  int verbosity;
  std::string refSequences;   // reference sequence(s)
  std::string querySequences; // query sequence(s)
};

}

#endif
