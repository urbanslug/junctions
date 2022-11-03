#include <cstdio>
#include<bits/stdc++.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "../../rs/src/junctions.h"

void print_edt(std::vector<std::vector<std::string>> ed_string) {
  for (auto degenerate_letter = ed_string.begin(); degenerate_letter < ed_string.end(); degenerate_letter++) {
    for (auto str = degenerate_letter->begin(); str < degenerate_letter->end(); str++) {
      std::cout << *str << " ";
    }
    std::cout << std::endl;
  }
}


void foo() {
  std::string strw = "Test string";

  const EdString ed_string = read_ed_string(strw.data(), strw.length() );
  const char **data = ed_string.data;
  const size_t *metadata = ed_string.metadata;

  std::vector<std::string> data_vec;
  std::vector<size_t> metadata_vec;

  for (; *data; ++data) {
    const char *i = *data;
    std::string str = i;

    // std::cout << str << std::endl;

    data_vec.push_back(str);
  }

  for (; *metadata; ++metadata) {
    size_t str_count = *metadata;

    // std::cout << str_count << std::endl;

    metadata_vec.push_back(str_count);
  }

  std::vector<std::vector<std::string>> string_sets;

  size_t counter = 0;
  // Declaring iterator to a vector
  std::vector<size_t>::iterator ptr;
  for (ptr = metadata_vec.begin(); ptr < metadata_vec.end(); ptr++) {
    size_t end = counter + *ptr;

    std::vector<std::string> v2 = std::vector<std::string>(data_vec.begin() + counter, data_vec.begin() + end);
    string_sets.push_back(v2);
    counter = counter + end;
  }

  print_edt(string_sets);

}



int main() {
  foo();
}
