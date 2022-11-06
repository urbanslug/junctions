#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <stdlib.h>
#include <string>
#include <vector>
#include <utility>

#include "./gen_suffix_tree.cpp"
#include "../../rs/src/junctions.h"


void print_str_vec(std::vector<std::string> &degenerate_letter);
void print_edt(std::vector<std::vector<std::string>> ed_string);


typedef std::vector<std::vector<std::string>> ed_string_data;
typedef std::vector<std::vector<bool>> matrix;
typedef std::vector<string> degenerate_letter; // strictly speaking this should be a set of strings

struct EDS {
  ed_string_data data;
  std::vector<size_t> n_offsets;
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

    std::vector<std::string> v2 =
      std::vector<std::string>(data_vec.begin() + counter,
                               data_vec.begin() + end);

    string_sets.push_back(v2);
    counter = end;
  }

  //size_t k = string_sets.size();

  std::vector<size_t> n_offsets;
  n_offsets.reserve(len);
  size_t index_in_n = 0;
  for (size_t i = 0; i < len; i++) {

    std::vector<std::string> v2 =  string_sets[i];
    for (auto s : v2) { index_in_n += s.length(); }

    n_offsets.push_back(index_in_n - 1);
  }

  EDS e;
  e.data = string_sets;
  e.length = len;
  e.size = size;
  e.n_offsets = n_offsets;

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

  // printf("=> %d\n", eds_w.n_offsets.size());
  
  for (size_t i=0; i<len_w; i++ ) {
    degenerate_letter i_letter = eds_w.data[i];

    // concat the strings
    std::string text;
    map<size_t, size_t> text_postitions; // end_idx string
    // std::vector<std::pair<size_t,size_t>> text_lengths(len_w);

    size_t prev_len = 0;
    for (size_t i = 0; i < i_letter.size(); i++) {
      std::string str = i_letter[i];
      text.append(str);

      text_postitions[prev_len + str.length() - 1] = i;
      // text_lengths.push_back(std::make_pair(i, prev_len + str.length() - 1));
      prev_len = prev_len + str.length();

    }

    text.push_back('_');

    printf("text: %s", text.c_str());
    for (auto t: text_postitions) {
      printf("\t%d -> %d", t.first, t.second);
    }

    printf("\n");

    // Create the suffix tree
    size_t text_length = text.length();
    STvertex *root = Create_suffix_tree(text.c_str(), text_length);

    // self spell
    std::vector<size_t> prefixes;
    for (size_t i = 0; i < i_letter.size(); i++) {
      std::string str = i_letter[i];
      size_t pos = FindEndIndex(str.c_str(), root, text.c_str()) - 1;

      if (pos == -1) {
          continue;
      }

      auto k = text_postitions.find(pos);
      if ((k == text_postitions.end()) || ((k->second != i))) {
          prefixes.push_back(pos);
          printf("text: %s, str: %s i: %d pos: %d\n", text.c_str(),
                 str.c_str(), i , pos);
        }
    }

    // spell j in i
    for (size_t j = 0; j < len_q; j++) {
      //std::string str = j_letter[j];
      degenerate_letter j_letter = eds_q.data[j];

      for (size_t j_str = 0; j_str < j_letter.size(); j_str++) {
        std::string str = j_letter[j];

        size_t pos = FindEndIndex(str.c_str(), root, text.c_str()) - 1;

        if (pos == -1) {
          continue;
        }

        auto k = text_postitions.find(pos);
        if (j == 0 || w_matrix[j - 1][pos - str.length() - 1] == 1) {
          w_matrix[j][pos] = 1;
        } 
      }
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
