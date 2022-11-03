#include <cstdint>
#include <stdio.h>


extern "C" void adder(std::size_t nrow, std::size_t ncol);


extern "C" void read_files(char const *w_path, std::size_t w_path_length,
                           char const *q_path, std::size_t q_path_length);





extern "C" char const* get_str();
extern "C" char const** get_strs();

struct EdString
{
  char const** data;
  size_t const* metadata;
};

extern "C"  EdString get_ed_string();

extern "C" EdString read_ed_string(char const* file_path, std::size_t w_path_length);
