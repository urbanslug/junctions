
#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <utility>

#include "./core.cpp"
#include "./gen_suffix_tree.cpp"


void test_lacks_intersect();
void test_contains_intersect();
void test_parse_ed_string();
void test_handle_epsilon();

namespace improved {
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
  bool intersect(EDS & eds_w, EDS & eds_q) {
    if (DEBUG_LEVEL > 1) {
      printf("[cpp::main::intersect]\n");
    }

    size_t size_w = eds_w.size;
    size_t size_q = eds_q.size;

    size_t len_w = eds_w.length;
    size_t len_q = eds_q.length;

    matrix w_matrix = gen_matrix(len_q, size_w);
    matrix q_matrix = gen_matrix(len_w, size_q);

    /*
      Preprocess suffix trees
      -----------------------
    */
    auto gen_suffix_tree =
        [](EDS &eds, size_t len,
           std::vector<std::pair<STvertex, std::string>> *w_suffix_trees) {
          size_t i = 0;
          while (i < len) {
            std::vector<std::string> i_letter = eds.data[i].data;
            std::string text;
            for (auto i_str : i_letter) {
              text.append(i_str);
            }                    // concat the strings
            text.push_back('_'); // add a terminator char

            // Create the suffix tree
            STvertex *root = Create_suffix_tree(text.c_str(), text.length());
            w_suffix_trees->push_back(std::make_pair(*root, text));

            ++(i);
          }
        };

    std::vector<std::pair<STvertex, std::string>> w_suffix_trees;
    w_suffix_trees.reserve(len_w);
    std::vector<std::pair<STvertex, std::string>> q_suffix_trees;
    q_suffix_trees.reserve(len_q);

    gen_suffix_tree(eds_w, len_w, &w_suffix_trees);
    gen_suffix_tree(eds_q, len_q, &q_suffix_trees);

    /*
      Find the intersection
      ---------------------
    */

    std::vector<int> q_end_indexes,
        w_end_indexes; // TODO: allocate one large one
    size_t i, j;
    i = j = 0;
    bool inc_i = false, inc_j = false;
    while (i < len_w && j < len_q) {
      if (DEBUG_LEVEL > 3) {
        printf("\tletter i: %lu, j: %lu\n", i, j);
        printf("\t----------------\n");
      }

      inc_i = inc_j = false;
      std::pair<STvertex, std::string> root_text;

      // -------
      // match in W
      // -------

      q_end_indexes.clear();
      root_text = q_suffix_trees[j];

      if (DEBUG_LEVEL > 3) {
        printf("\tj text: %s\n", root_text.second.c_str());
      }

      // handle epsilon
      if (eds_w.data[i].has_epsilon &&
          (j == 0 || is_letter_matched(eds_w, i - 1, w_matrix, j))) {
        std::vector<span> letter_span = eds_w.str_offsets[i];
        int epsilon_idx = letter_span.back().stop;

        w_matrix[j][epsilon_idx] = true;
        q_end_indexes.push_back(epsilon_idx);
        inc_i = true;
      }

      for (auto i_str : eds_w.data[i].data) {
        // if (DEBUG_LEVEL > 3) { printf("\t\ti_str: %s\n", i_str.c_str()); }

        vector<int> res = FindEndIndexes(i_str.c_str(), &root_text.first,
                                         root_text.second.c_str());

        // if the string does not match anywhere
        if (res.empty()) {
          continue;
        }

        std::vector<span> spans = eds_q.str_offsets[j];
        int letter_start_in_n = spans.front().start;

        for (int pos : res) {

          int pos_in_n = letter_start_in_n + pos;

          if (DEBUG_LEVEL > 3) {
            printf("\t\ti_str: %s pos: %d pos_in_n: %d\n", i_str.c_str(), pos,
                   pos_in_n);
          }

          // if (!is_match_valid(spans, j, i_str.length(), pos,
          // letter_start_in_n)) { continue; }

          if (is_match_valid(spans, j, i_str.length(), pos,
                             letter_start_in_n) &&
              (i == 0 || j == 0 || // TODO: is this check robust?
               (is_pos_at_start(spans, pos_in_n) &&
                is_letter_matched(eds_q, j - 1, q_matrix, i)) ||
               is_prefix_matched(eds_q, q_matrix, pos, i, j))) {
            int end_idx = pos_in_n + i_str.length() - 1;
            q_matrix[i][end_idx] = true;
            q_end_indexes.push_back(end_idx);
            inc_i = true;

            if (DEBUG_LEVEL > 4) {
              printf("\t\tpos %d  \tpos_in_n: %d\n", pos, end_idx);
            }
          }
        }
      }

      // -------
      // match in Q
      // -------

      w_end_indexes.clear();
      root_text = w_suffix_trees[i];
      if (DEBUG_LEVEL > 3) {
        printf("\ti text: %s\n", root_text.second.c_str());
      }

      // handle epsilon
      if (eds_q.data[j].has_epsilon &&
          (i == 0 || is_letter_matched(eds_q, j - 1, q_matrix, i))) {
        std::vector<span> letter_span = eds_q.str_offsets[j];
        int epsilon_idx = letter_span.back().stop;

        q_matrix[i][epsilon_idx] = true;
        w_end_indexes.push_back(epsilon_idx);
        inc_j = true;
      }

      std::vector<std::string> j_strings = eds_q.data[j].data;

      for (auto j_str : j_strings) {

        vector<int> res = FindEndIndexes(j_str.c_str(), &root_text.first,
                                         root_text.second.c_str());

        // if the string does not match anywhere
        if (res.empty()) {
          continue;
        }

        if (DEBUG_LEVEL > 3) {
          printf("\t\tj_str: %s\n", j_str.c_str());
        }

        std::vector<span> spans = eds_w.str_offsets[i];
        int letter_start_in_n = spans.front().start;

        for (int pos : res) {

          if (DEBUG_LEVEL > 3) {
            printf("\t\t\tpos: %d\n", pos);
          }

          // if (!is_match_valid(spans, i, j_str.length(), pos,
          // letter_start_in_n)) { continue; }

          int pos_in_n = letter_start_in_n + pos;

          if (is_match_valid(spans, i, j_str.length(), pos,
                             letter_start_in_n) &&
              (j == 0 || i == 0 || // TODO: is this check robust enough?
               (is_pos_at_start(spans, pos_in_n) &&
                is_letter_matched(eds_w, i - 1, w_matrix,
                                  j)) || // TODO: what if i == 0?
               is_prefix_matched(eds_w, w_matrix, pos, j, i))) {
            int end_idx = pos_in_n + j_str.length() - 1;
            w_matrix[j][end_idx] = true;

            w_end_indexes.push_back(end_idx);
            inc_j = true;

            if (DEBUG_LEVEL > 4) {
              printf("\t\t\tset j %lu \tpos_in_n: %d\n", j, end_idx);
            }
          }
        }
      }

      // if allowed, increment the row and copy over the previously matched rows
      auto inc_and_copy = [](matrix &dp_matrix, std::size_t *row, int max_row,
                             std::vector<int> &cols) {
        ++(*row);
        if (*row < max_row) {
          for (int k : cols) {
            dp_matrix[*row][k] = dp_matrix[*row - 1][k];
          }
        }
      };

      if (inc_i) {
        inc_and_copy(q_matrix, &i, len_w, q_end_indexes);
      }

      if (inc_j) {
        inc_and_copy(w_matrix, &j, len_q, w_end_indexes);
      }

      // give up early because we can not extend any degenerate letter
      if (!inc_j && !inc_i) {
        return false;
      }
    }

    // computing accepting states
    std::vector<std::size_t> w_ends = compute_str_ends(eds_w, len_w - 1);
    std::vector<std::size_t> q_ends = compute_str_ends(eds_q, len_q - 1);

    // TODO: rename
    auto foo = [](matrix &m, std::vector<size_t> ends, size_t row) -> bool {
      for (auto end : ends) {
        if (m[row][end]) {
          return true;
        }
      }

      return false;
    };

    bool accept_w = foo(w_matrix, w_ends, len_q - 1);
    bool accept_q = foo(q_matrix, q_ends, len_w - 1);

    return accept_w && accept_q;
  }
}

namespace naive {

  /*
                 q
        0   ...    j   ...   n
        |--------------------|
      . |                    |
      . |                    |
      . |                    |
    w . |                    |
      i |                    |
      . |                    |
      . |                    |
      . |                    |
        |--------------------|
        m
  */

  bool intersect(EDS &eds_w, EDS &eds_q) {
    if (DEBUG_LEVEL > 1) { printf("[main::naive::intersect]\n"); }

    LinearizedEDS linear_w =  parser::linearize(eds_w);
    LinearizedEDS linear_q =  parser::linearize(eds_q);

    size_t last_row = linear_w.str.length();
    size_t last_col = linear_q.str.length();

    boolean_matrix dp_matrix = gen_matrix(last_row, last_col);
    // boolean_matrix q_dp_matrix = gen_matrix(last_row, last_col);

    auto prev_matched =
      [&dp_matrix, &linear_w, &linear_q](int row, int col) -> bool {
      std::vector<int> prev_w = linear_w.prev_chars[row];
      std::vector<int> prev_q = linear_q.prev_chars[col];

      // TODO: what if only one is empty?
      if (prev_w.empty() && prev_q.empty() ) { return true; }

      // std::cout << row << " " << col << "\n";

      for (int prev_row_idx : prev_w) {
        for (int prev_col_idx: prev_q) {
          if (dp_matrix[prev_row_idx][prev_col_idx]) { return true; }
        }
      }

      return false;
    };

    auto chars_match = [ &linear_w, &linear_q](int row_idx, int col_idx) -> bool {
      return linear_w.str[row_idx] == linear_q.str[col_idx];
    };

    for (size_t row_idx = 0; row_idx < last_row; row_idx++) {
      for (size_t col_idx = 0; col_idx < last_col; col_idx++) {
        if (chars_match(row_idx, col_idx) && prev_matched(row_idx, col_idx)) {
            dp_matrix[row_idx][col_idx] = true;
        }

        if (linear_w.str[row_idx] == '*') {
          int_vec prev_w = linear_w.prev_chars[row_idx];

          for (int r : prev_w) {
            for (int c = 0; c <= col_idx; c++) {
              if (dp_matrix[r][c]) {
                dp_matrix[row_idx][c] = true;
              }
            }
          }
        }

        if (linear_q.str[col_idx] == '*') {

          int_vec prev_q = linear_q.prev_chars[col_idx];
          for (int c : prev_q) {
            for (int r = 0; r <= row_idx; r++ ) {
              if (dp_matrix[r][c]) {
                dp_matrix[r][col_idx] = true;
              }
            }
          }
        }
      }
    }

    size_t_vec accept_q = compute_accepting_states(eds_q);
    size_t_vec accept_w = compute_accepting_states(eds_w);

    for(int row: accept_w) {
      for (int col: accept_q) {
        if (dp_matrix[row][col]) {
          return true;
        }
      }
    }

    return false;
  }
}

int main() {
  // test_handle_epsilon();
  test_contains_intersect();
  test_lacks_intersect();
  test_parse_ed_string();
  
  return 0;
}

/*
  Tests
  -----
*/

// If parameter is not true, test fails
// This check function would be provided by the test framework
#define IS_TRUE(x) { if (!(x)) std::cout << __FUNCTION__ << " failed on line " << __LINE__ << std::endl;}


void test_parse_ed_string() {
  std::string ed_string;
  EDS eds;

  // TODO: confirm size, len, offsets etc for all

  ed_string = "{AT,TC}{ATC,T}";
  eds = parse_ed_string(ed_string);
  // print_edt(eds);

  ed_string = "{AT,TC}{ATC,}";
  eds = parse_ed_string(ed_string);
  // print_edt(eds);

  ed_string = "ACTGAC{AT,,TC}AGG{,ATC,}CT{AT,TC}A";
  // ed_string = "ACC{AT,,TC}AGG";
  eds = parse_ed_string(ed_string);
  // print_edt(eds);

  ed_string = "ACTGACCT";
  eds = parse_ed_string(ed_string);
  // print_edt(eds);

  ed_string = "{AT,TC}{ATC,T}";
  eds = parse_ed_string(ed_string);
  // print_edt(eds);

  ed_string = "{,G}{CT,T}";
  eds = parse_ed_string(ed_string);
  IS_TRUE(eds.data[0].has_epsilon);
  IS_TRUE(!eds.data[1].has_epsilon);
  IS_TRUE(eds.length == 2);
  // print_edt(eds);

  std::vector<std::vector<span>> str_offsets = eds.str_offsets;
}

void test_contains_intersect() {
  std::string ed_string_w = "{AT,TC}{ATC,T}";
  std::string ed_string_q = "{TC,G}{CT,T}";

  EDS eds_w = parse_ed_string(ed_string_w);
  EDS eds_q = parse_ed_string(ed_string_q);

  // IS_TRUE(improved::intersect(eds_w, eds_q));
  IS_TRUE(naive::intersect(eds_w, eds_q) == improved::intersect(eds_w, eds_q));
}

void test_handle_epsilon() {
  std::string ed_string_w, ed_string_q;
  EDS eds_w, eds_q;

  ed_string_w = "{AT,TC}{TC,T}";
  ed_string_q = "TC{,G}{CT,T}";
  eds_w = parse_ed_string(ed_string_w);
  eds_q = parse_ed_string(ed_string_q);
  // IS_TRUE(improved::intersect(eds_w, eds_q));
  /*
  std::cout << "naive: "     << naive::intersect(eds_w, eds_q)
            << " improved: " << improved::intersect(eds_w, eds_q)
            << std::endl;
            */
  // IS_TRUE(naive::intersect(eds_w, eds_q) == improved::intersect(eds_w, eds_q));

  ed_string_w = "{AT,TC}{ATC,}";
  ed_string_q = "{TC,G}{CT,T}";
  eds_w = parse_ed_string(ed_string_w);
  eds_q = parse_ed_string(ed_string_q);
  // IS_TRUE(!improved::intersect(eds_w, eds_q));
  // IS_TRUE(naive::intersect(eds_w, eds_q) == improved::intersect(eds_w, eds_q));

  ed_string_w = "{AT,TC}{ATC,T}";
  ed_string_q = "{,G}AT{CT,T}";
  eds_w = parse_ed_string(ed_string_w);
  eds_q = parse_ed_string(ed_string_q);
  // IS_TRUE(improved::intersect(eds_w, eds_q));
  // IS_TRUE(naive::intersect(eds_w, eds_q) == improved::intersect(eds_w, eds_q));

  ed_string_w = "{AT,TC}{ATC,A}";
  ed_string_q = "{,G}{CT,T}";
  eds_w = parse_ed_string(ed_string_w);
  eds_q = parse_ed_string(ed_string_q);
  // IS_TRUE(!improved::intersect(eds_w, eds_q));
  // IS_TRUE(naive::intersect(eds_w, eds_q) == improved::intersect(eds_w, eds_q));

  ed_string_w = "{AT,TC}{CGA,}{AGC,ATGC,}{ATC,T}";
  ed_string_q = "{TC,G}{CT,T}";
  eds_w = parse_ed_string(ed_string_w);
  eds_q = parse_ed_string(ed_string_q);

  std::cout << "naive: "     << naive::intersect(eds_w, eds_q)
            << " improved: " << improved::intersect(eds_w, eds_q)
            << std::endl;
  // IS_TRUE(naive::intersect(eds_w, eds_q) == improved::intersect(eds_w, eds_q));
}

void test_lacks_intersect() {
  std::string ed_string_w = "{AT,TC}{ATC,T}";
  std::string ed_string_q = "{TC,G}{CG,G}";

  EDS eds_w = parse_ed_string(ed_string_w);
  EDS eds_q = parse_ed_string(ed_string_q);

  // IS_TRUE(!improved::intersect(eds_w, eds_q));
  IS_TRUE(naive::intersect(eds_w, eds_q) == improved::intersect(eds_w, eds_q));
}

