#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>


namespace core::constants {
// current version of junctions
const std::string JUNCTIONS_VERSION = "0.0.0-alpha";

// TODO [c] move to core::constants
const std::string unicode_eps = "\u03B5";
const std::string unicode_sub_1 = "\u2081";
const std::string unicode_sub_2 = "\u2082";

const std::string T_1 = "T\u2081";
const std::string T_2 = "T\u2082";
const std::string N_1 = "N\u2081";
const std::string N_2 = "N\u2082";
const std::string q_0 = "q\u2080";
const std::string q_a = "q\u2090";

const char string_separator = '$';
const char terminator_char = '_';
}
#endif
