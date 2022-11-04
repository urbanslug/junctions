#include <cstdio>
#include<bits/stdc++.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "./gen_suffix_tree.cpp"
#include "../../rs/src/junctions.h"


void print_str_vec(std::vector<std::string> &degenerate_letter);
void print_edt(std::vector<std::vector<std::string>> ed_string);


typedef std::vector<std::vector<std::string>> ed_string_data;
typedef std::vector<std::vector<bool>> matrix;
typedef std::vector<string> degenerate_letter; // strictly speaking this should be a set of strings

struct EDS {
  ed_string_data data;
  size_t size;
  size_t length ;
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
  size_t size = ed_string.size;

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
  e.length = len;
  e.size = size;

  return e;
}

matrix gen_matrix(size_t rows, size_t cols) {
  printf("[cpp::main::gen_matrix]\n");
  // rows
  std::vector<bool> row(cols);
  fill(row.begin(), row.end(), false);

  // the matrix
  std::vector<std::vector<bool>> actual_matrix(rows);

  fill(actual_matrix.begin(), actual_matrix.end(), row);

  /*
   Print matrix
   matrix m = actual_matrix;

   for (auto row = m.begin(); row < m.end(); row++) {
   for (bool b: *row )
   std::cout << "v" << std::endl;
   std::cout << std::endl;
   }

   */

  return actual_matrix;
}

// Is there an intersection between ED string w and ED String q?
bool intersect(EDS &eds_w, EDS &eds_q) {
  printf("[cpp::main::intersect]\n");
  // Gen the matrices for...

  size_t size_w = eds_w.size;
  size_t size_q = eds_q.size;

  size_t len_w =  eds_w.length;
  size_t len_q =  eds_q.length;

  matrix w_matrix = gen_matrix(len_w, size_q);
  matrix q_matrix = gen_matrix(len_q, size_w);

  for (size_t i=0; i<len_w; i++ ) {
    degenerate_letter i_letter = eds_w.data[i];

    std::string concat_string;

      for (std::string i: i_letter) {
        concat_string.append(i);
      }
  }

  return w_matrix[len_w-1][size_q-1] && q_matrix[len_q-1][size_w-1];
}

int main() {
  // ed string in eds format
  std::string ed_string_w = "{AT,TC}{ATC,T}";
  std::string ed_string_q = "{TC,G}{CT,T}";

  EDS eds_w = parse_ed_string(ed_string_w);
  EDS eds_q = parse_ed_string(ed_string_q);


  bool found = intersect(eds_w, eds_q);

  if (found) {
      printf("contains intersect\n");
    } else {
    printf("lacks intersect\n");
  }

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
