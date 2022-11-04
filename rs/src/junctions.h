#include <cstdint>
#include <stdio.h>


struct EdString
{
  char const** data;
  size_t const* metadata;
  size_t metadata_len;
};

extern "C" EdString read_ed_string(char const* file_path, std::size_t w_path_length);

extern "C" char const** pass_string();
