#ifndef UTILS_HPP
#define UTILS_HPP

#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <set>
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>

#include "core.hpp"



namespace utils {
const uint8_t DEBUG_LEVEL = 0; // TODO: replace with verbosity in params
std::string indent(int level);
void print_edt(EDS &ed_string);
boolean_matrix gen_matrix(size_t rows, size_t cols);
void print_matrix(boolean_matrix const &m);
std::vector<std::size_t> compute_accepting_states(EDS &eds);
}



#endif
