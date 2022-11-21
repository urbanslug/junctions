#include <fstream>
#include <iostream>
#include <string>
#include <vector>
// using namespace std;

namespace files {
std::string read_eds(std::string &file_path) {
  std::string line, eds_string;

  std::ifstream myfile(file_path);
  if (myfile.is_open()) {
    while (getline(myfile, line)) {
      eds_string.append(line);
    }
    myfile.close();
  } else
    std::cerr << "[files::read_eds] Error parsing: " << file_path << std::endl;

  return eds_string;
}

std::vector<std::string> read_msa(std::string &file_path) {
  std::string line;
  std::vector<std::string> msa_data;
  bool has_header = false;

  std::ifstream myfile(file_path);
  if (myfile.is_open()) {
    while (getline(myfile, line)) {
      if (!has_header && msa_data.empty()) {
        if (line[0] == '>') {
          has_header = true;
          continue;
        } else {
          std::cerr << "[files::read_msa] no header file" << file_path
                    << std::endl;
        }
      }
      msa_data.push_back(line);
    }
    myfile.close();
  } else
    std::cerr << "[files::read_msa] Error parsing: " << file_path << std::endl;

  return msa_data;
}
}

