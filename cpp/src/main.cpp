#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <stdlib.h>
#include <string>
#include <vector>
#include <utility>

#include "./gen_suffix_tree.cpp"
#include "../../rs/src/junctions.h"

const uint8_t DEBUG_LEVEL = 1;

void print_str_vec(std::vector<std::string> &degenerate_letter);
void print_edt(std::vector<std::vector<std::string>> ed_string);


typedef std::vector<std::vector<std::string>> ed_string_data;
typedef std::vector<std::vector<bool>> matrix;
typedef std::vector<string> degenerate_letter; // strictly speaking this should be a set of strings

struct span {
  size_t start;
  size_t stop;
};

struct EDS {
  ed_string_data data;
  std::vector<span> n_offsets; // 
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

  std::vector<span> n_offsets;
  n_offsets.reserve(len);
  size_t index_in_n = 0;
  for (size_t i = 0; i < len; i++) {

    
    span s;
    s.start = index_in_n;
    std::vector<std::string> v2 = string_sets[i];
    for (auto s : v2) { index_in_n += s.length(); }

    s.stop = index_in_n - 1;
    n_offsets.push_back(s);
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

// compute string ends indexes (0 indexed) given an eds and letter
std::vector<std::size_t> compute_str_ends(EDS &eds, size_t letter) {
  degenerate_letter final_i_letter = eds.data[letter];

  std::vector<std::size_t> accepting_states;
  accepting_states.reserve(final_i_letter.size());

  // start of the degenerate letter in N
  std::size_t tracker = eds.n_offsets[letter].start;
  for (auto str : final_i_letter) {
    accepting_states.push_back(tracker + str.length() - 1);
    tracker += str.length();
  }

  return accepting_states;
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

  if (DEBUG_LEVEL > 3) {
      // printf("=> %lu\n", eds_w.n_offsets.size());
      for (auto o : eds_w.n_offsets)
        printf("=> (%lu, %lu)\n", o.start, o.stop);
  }


  /*
    preprocess suffix trees
    -----------------------
  */
  std::vector<STvertex>
    w_suffix_trees;
  w_suffix_trees.reserve(len_w);
  std::vector<STvertex> q_suffix_trees;
  q_suffix_trees.reserve(len_q);

  size_t i = 0; size_t j = 0;
  while (i < len_q && j < len_w) {
    if (i < len_q) {
      degenerate_letter i_letter = eds_w.data[i];
      std::string text;
      for (auto i_str : i_letter) { text.append(i_str); } // concat the strings
      text.push_back('_'); // add a terminator char

      // Create the suffix tree
      STvertex *root = Create_suffix_tree(text.c_str(), text.length());
      w_suffix_trees.push_back(*root);

      ++i;
    }

    if (j < len_w) {
      degenerate_letter j_letter = eds_w.data[j];

      std::string text;
      for(auto j_str: j_letter) { text.append(j_str); } // concat the strings
      text.push_back('_'); // add a terminator char

      // Create the suffix tree
      STvertex *root = Create_suffix_tree(text.c_str(), text.length());
      q_suffix_trees.push_back(*root);

      ++j;
    }
  }

  /*
    Find the intersection
   */
  i = j = 0; // reset i and j
  while (i < len_q && j < len_w) {
    break;
  }

  // computing accepting states
  std::vector<std::size_t> accept_w = compute_str_ends(eds_w, len_w - 1);
  std::vector<std::size_t> accept_q = compute_str_ends(eds_q, len_q - 1);
  

  for (auto state_i : accept_w) {
    for (auto state_j: accept_q) {
      if (w_matrix[len_q-1][state_i] == 1 && q_matrix[len_w-1][state_j] == 1) { return true; }
    }
  }

  return false;
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


// utils

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
