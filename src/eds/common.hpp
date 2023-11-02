#ifndef COMMON_EDS_HPP
#define COMMON_EDS_HPP

#include <string>



namespace eds {
#define FILE_ERROR(name)                                                       \
  {                                                                            \
	std::string e = "Error, Failed to open the file" + name; \
	throw std::invalid_argument(e);                                              \
  }

/**
 * Get the size of a file
 */
std::size_t get_file_size(const std::string& fp);

}
#endif
