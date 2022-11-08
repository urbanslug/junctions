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

void test_lacks_intersect();
void test_contains_intersect();

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
// within N
std::vector<std::size_t> compute_str_ends(EDS &eds, size_t letter) {
  printf("[cpp::main::compute_str_ends]\n");
  // printf("letter: %lu", letter);
  degenerate_letter final_i_letter = eds.data[letter];



  std::vector<std::size_t> accepting_states;
  accepting_states.reserve(final_i_letter.size());

  // start of the degenerate letter in N
  std::size_t tracker = eds.n_offsets[letter].start;
  for (auto str : final_i_letter) {
    accepting_states.push_back(tracker + str.length() - 1);
    tracker += str.length();
  }

  /*
  for (auto end : accepting_states)
    printf("%lu", end);
  */
  return accepting_states;
}

/*
  Is there an intersection between ED strings W and Q?

  W matrix: This matrix tracks how far in N we spelled strings in W


           0 |<------- i ------>| n

      |-      -| |-    -|     |-      -|
      | ...... | |   .  |     |   ...  |
  W = |  ....  | | .... | ... | ...... |
      | ...... | |  ..  |     |   ..   |
      |-      -| |-    -|     |-      -|

  0                             N
  |-----------------------------|
  |                             |
  |                             |
  |          W_matrix           |
  |                             |
  |-----------------------------|
 m


    Q matrix: This matrix tracks how far in M we spelled strings in Q

          0 |<-------- j ------->| m

      |-    -| |-      -|     |-       -|
      |  ..  | |  ....  |     | ....... |
  Q = | .... | |    .   | ... |   ..    |
      |  ..  | | ...... |     | ....... |
      |-    -| |-      -|     |-       -|


  0                             M
  |-----------------------------|
  |                             |
  |                             |
  |          Q_matrix           |
  |                             |
  |-----------------------------|
  n
*/
bool intersect(EDS &eds_w, EDS &eds_q) {
  printf("[cpp::main::intersect]\n");

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
    Preprocess suffix trees
    -----------------------
  */
  std::vector<std::pair<STvertex, std::string>> w_suffix_trees;
  w_suffix_trees.reserve(len_w);
  std::vector<std::pair<STvertex, std::string>> q_suffix_trees;
  q_suffix_trees.reserve(len_q);

  size_t i = 0; size_t j = 0;
  while (i < len_q && j < len_w) {
    // TODO: offload to lambda
    if (i < len_q) {
      degenerate_letter i_letter = eds_w.data[i];
      std::string text;
      for (auto i_str : i_letter) { text.append(i_str); } // concat the strings
      text.push_back('_'); // add a terminator char

      // Create the suffix tree
      STvertex *root = Create_suffix_tree(text.c_str(), text.length());
      w_suffix_trees.push_back(std::make_pair(*root, text));

      ++i;
    }

    if (j < len_w) {
      degenerate_letter j_letter = eds_q.data[j];

      std::string text;
      for(auto j_str: j_letter) { text.append(j_str); } // concat the strings
      text.push_back('_'); // add a terminator char

      // Create the suffix tree
      STvertex *root = Create_suffix_tree(text.c_str(), text.length());
      q_suffix_trees.push_back(std::make_pair(*root, text));

      ++j;
    }
  }

  /*
    Find the intersection
    ---------------------
   */
  i = j = 0; // reset i and j
  bool inc_i = false, inc_j = false;
  while (i < len_q && j < len_w) {
    printf("i: %lu, j: %lu\n", i, j);

    inc_i = inc_j = false;
    std::pair<STvertex, std::string> root_text;


    root_text = q_suffix_trees[j];
    printf("\ti letter: %lu\n", i);
    printf("\t\ttext: %s\n", root_text.second.c_str());
    for (auto i_str : eds_w.data[i]) {
      printf("\t\ti_str: %s\n", i_str.c_str());

      int pos = FindEndIndex(i_str.c_str(), &root_text.first, root_text.second.c_str());

      if (pos < 0) { continue; } // if pos is not found

      if (i == 0 ) {
        q_matrix[i][pos] = 1;
        inc_i = true;
        continue;
      }

      // did we get to the end of the previous degenerate letter
      bool finished_prev = true;


      std::vector<std::size_t> prev_ends = compute_str_ends(eds_q, j - 1);
      for (auto end : prev_ends) {
        if (q_matrix[i - 1][end] == 1) {
          finished_prev = true;
          break;
        }
      }

      if (finished_prev) {
        size_t pos_in_n = eds_q.n_offsets[j].start + pos;
        printf("\t\ti_str %s  text %s  pos %d  q_matrix pos_in_n: %lu\n",
               i_str.c_str(), root_text.second.c_str(), pos, pos_in_n);
        q_matrix[i][pos_in_n] = 1;
        inc_i = true;
      }
    }

    root_text = w_suffix_trees[i];
    printf("\tj letter: %lu\n", j);
    printf("\t\ttext: %s\n", root_text.second.c_str());
    for (auto j_str : eds_q.data[j]) {
      printf("\t\tj_str: %s\n", j_str.c_str());

      int pos = FindEndIndex(j_str.c_str(), &root_text.first, root_text.second.c_str());

      if (pos < 0) { continue; } // if pos is not found

      if (j == 0) {
        w_matrix[j][pos] = 1;
        inc_j = true;
        continue;
      }

      // did we get to the end of the previous degenerate letter
      bool finished_prev = false;
      std::vector<std::size_t> prev_ends = compute_str_ends(eds_w, i - 1);
      for (auto end: prev_ends) {
        if (w_matrix[j - 1][end] == 1) {
          finished_prev = true;
          break;
        }
      }

      if (finished_prev) {
        size_t pos_in_n = eds_w.n_offsets[i].start + pos;
        printf("\t\tj_str %s \t pos %d \t w_matrix pos_in_n: %lu\n", j_str.c_str(), pos, pos_in_n);
        w_matrix[j][pos_in_n] = 1;
        inc_j = true;
      }
    }

    if (inc_i) { ++i; }

    if (inc_j) { ++j; }

    // give up early we cant extend any degenerate letter
    if (!inc_j && !inc_i) { return false; }
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
  test_contains_intersect();
  // test_lacks_intersect();
  return 0;
}

/*
  utils
  -----
 */
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

/*
  Tests
  -----
 */

// If parameter is not true, test fails
// This check function would be provided by the test framework
#define IS_TRUE(x) { if (!(x)) std::cout << __FUNCTION__ << " failed on line " << __LINE__ << std::endl;}

// Test for function1()
// You would need to write these even when using a framework
void test_contains_intersect() {
  std::string ed_string_w = "{AT,TC}{ATC,T}";
  std::string ed_string_q = "{TC,G}{CT,T}";

  EDS eds_w = parse_ed_string(ed_string_w);
  EDS eds_q = parse_ed_string(ed_string_q);

  IS_TRUE(intersect(eds_w, eds_q));
}

void test_lacks_intersect() {
  std::string ed_string_w = "{AT,TC}{ATC,T}";
  std::string ed_string_q = "{TC,G}{CG,G}";

  EDS eds_w = parse_ed_string(ed_string_w);
  EDS eds_q = parse_ed_string(ed_string_q);

  IS_TRUE(!intersect(eds_w, eds_q));
}

