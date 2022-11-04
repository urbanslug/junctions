#include <cstdio>
#include<bits/stdc++.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "../../rs/src/junctions.h"

void print_str_vec(std::vector<std::string> &degenerate_letter);
void print_edt(std::vector<std::vector<std::string>> ed_string);


typedef std::vector<std::vector<std::string>> ed_string_data;

struct EDS {
  ed_string_data data;
};


void bar() {
  const char** data = pass_string();

  for (; *data; ++data) {
    const char *i = *data;
    std::string str = i;

    std::cout << str << std::endl;

  }
}

// offload to rust parser
EDS parse_ed_string(std::string &eds) {

  printf("[cpp::main::parse_ed_string]\n");

  const EdString ed_string = read_ed_string(eds.data(), eds.length() );
  const char** data = ed_string.data;
  const size_t* metadata = ed_string.metadata;
  size_t len = ed_string.metadata_len;

  std::vector<std::string> data_vec;
  std::vector<size_t> metadata_vec;

  for (; *data; ++data) {
    const char *i = *data;
    std::string str = i;

    // std::cout << str << std::endl;

    data_vec.push_back(str);
  }


  // printf("%p ", data);
  // printf("%p ", metadata);

  for (size_t i=0; i < len; i++, ++metadata) {
    size_t str_count = *metadata;

    // std::cout << str_count << std::endl;

    metadata_vec.push_back(str_count);
  }

  std::vector<std::vector<std::string>> string_sets;

  size_t counter = 0;
  // Declaring iterator to a vector
  std::vector<size_t>::iterator ptr;
  for (ptr = metadata_vec.begin(); ptr <= metadata_vec.end(); ptr++) {
    size_t end = counter + *ptr;

    // std::cout << end << std::endl;


    std::vector<std::string> v2 = std::vector<std::string>(data_vec.begin() + counter,
                                                           data_vec.begin() + end);

    string_sets.push_back(v2);
    counter = end;
  }

  print_edt(string_sets);

  EDS e;
  e.data = string_sets;

  return e;

}

int main() {
  // ed string in eds format
  std::string raw_eds = "{AT,TC}{ATC,T}";

  EDS eds = parse_ed_string(raw_eds);
  // bar();
  return 0;
}


void print_edt(std::vector<std::vector<std::string>> ed_string) {
  for (auto degenerate_letter = ed_string.begin(); degenerate_letter < ed_string.end(); degenerate_letter++) {
    for (auto str = degenerate_letter->begin(); str < degenerate_letter->end(); str++) {
      std::cout << *str << " ";
    }
    std::cout << std::endl;
  }
}

void print_str_vec(std::vector<std::string> &degenerate_letter) {
    for (auto str = degenerate_letter.begin(); str < degenerate_letter.end(); str++) {
      std::cout << *str << " ";
    }
}
