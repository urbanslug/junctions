#include "utils.hpp"

namespace utils {
core::file_format extract_extension(std::string const& file_path) {
  std::string extension = file_path.substr(file_path.find_last_of(".") + 1);  

  if (extension == "eds") { return core::eds; }

  if (extension == "msa") { return core::msa; }

  // TODO: throw exception
  std::cerr << "ERROR,Could not determine the format of "
			<< file_path
			<< "\nThis input should be an ED-string file in .eds format"
			<< std::endl;
  exit(1);
  
}
}
