#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>

const uint8_t DEBUG_LEVEL = 1;

typedef std::vector<std::vector<std::string>> ed_string_data;
typedef std::vector<std::vector<bool>> matrix;
// strictly speaking this should be a set of strings
typedef std::vector<std::string> degenerate_letter;



struct span {
  size_t start;
  size_t stop;
};

struct EDS {
  ed_string_data data;
  std::vector<std::vector<span>> str_offsets;
  size_t size;
  size_t length;
};
