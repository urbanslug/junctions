#include <iostream>
#include <iomanip>
#include <fstream>

#include "./common.hpp"

namespace eds {

/**
 * Get the size of a file
 */
std::size_t get_file_size(const std::string& fp) {
  std::streampos begin, end;
  std::ifstream f (fp);

  if (!f) { FILE_ERROR(fp); }

  begin = f.tellg();
  f.seekg (0, std::ios::end);
  end = f.tellg();
  f.close();

  return end-begin;
}
}
