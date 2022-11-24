#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>

#include "./parser.cpp"
#include "./suffix_tree.cpp"
#include "core.hpp"

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

bool intersect(EDS & eds_w, EDS & eds_q, core::Parameters parameters) {
  if (parameters.verbosity > 1) {
    printf("INFO, [improved::intersect]\n");
  }

  size_t size_w = eds_w.size;
  size_t size_q = eds_q.size;

  size_t len_w = eds_w.length;
  size_t len_q = eds_q.length;

  if (parameters.verbosity > 2) {
    std::cerr << "INFO, [improved::intersect] N: " << size_w << " n: " << len_w
              << " M: " << size_q << " n: " << len_q << std::endl;
  }
    
  matrix w_matrix = utils::gen_matrix(len_q, size_w);
  matrix q_matrix = utils::gen_matrix(len_w, size_q);

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

  std::vector<int> q_end_indexes, w_end_indexes; // TODO: allocate one large one
  size_t i, j;
  i = j = 0;
  bool inc_i = false, inc_j = false;

  // the starts of these suffixes
  std::vector<std::pair<int, int>> i_valid_suffixes,
    j_valid_suffixes, next_i_valid_suffixes, next_j_valid_suffixes;

  std::vector<span> j_offsets = eds_q.str_offsets[j];
  for (int str_idx = 0; str_idx < j_offsets.size(); str_idx++) {
    if (eds_q.data[j].has_epsilon && str_idx == j_offsets.size() -1) { break; }
    int internal_start = j_offsets[str_idx].start - j_offsets.front().start;
    j_valid_suffixes.push_back(std::make_pair(str_idx, 0));
  }

  std::vector<span> i_offsets = eds_w.str_offsets[i];
  for (int str_idx = 0; str_idx < i_offsets.size(); str_idx++) {
    if (eds_w.data[i].has_epsilon && str_idx == i_offsets.size() - 1) { break; }
    int internal_start = i_offsets[str_idx].start - i_offsets.front().start;
    i_valid_suffixes.push_back(std::make_pair(str_idx, 0));
  }

  while (i < len_w && j < len_q) {
    if (parameters.verbosity > 3) {
      printf("\tletter i: %lu, j: %lu\n", i, j);
      printf("\t----------------\n");
    }

    next_i_valid_suffixes.clear();
    next_j_valid_suffixes.clear();
    inc_i = inc_j = false;
    std::pair<STvertex, std::string> root_text;

    // -------
    // match in W
    // -------

    // q_end_indexes.clear();
    root_text = q_suffix_trees[j];

    if (parameters.verbosity > 3) { printf("\tj text: %s\n", root_text.second.c_str()); }

    for (std::string str : eds_q.data[j].data) { std::cerr << str << "."; }
    if (eds_q.data[j].has_epsilon) { std::cerr << "."; }
    std::cerr << std::endl;

    // handle epsilon
    if (eds_w.data[i].has_epsilon && (j == 0 || utils::is_letter_matched(eds_w, i - 1, w_matrix, j))) {
      std::vector<span> letter_span = eds_w.str_offsets[i];
      int epsilon_idx = letter_span.back().stop;

      w_matrix[j][epsilon_idx] = true;
      w_end_indexes.push_back(epsilon_idx);

      inc_i = true;
    }

    for (auto suff : i_valid_suffixes) {
      // if (parameters.verbosity > 3) { printf("\t\ti_str: %s\n", i_str.c_str()); }
      // std::cerr << "fir " << suff.first << " sec " << suff.second << std::endl;
      // std::cerr << "full str " << eds_w.data[i].data[suff.first] << std::endl;

      std::string i_str = eds_w.data[i].data[suff.first].substr(suff.second);

      vector<int> res = FindEndIndexes(i_str.c_str(), &root_text.first, root_text.second.c_str());

      // if the string does not match anywhere
      if (res.empty()) { continue; }

      if (parameters.verbosity > 3) { std::cerr << "\t\ti_str: " << i_str << std::endl; }

      std::vector<span> spans = eds_q.str_offsets[j];
      int letter_start_in_n = spans.front().start;

      for (int pos : res) {

        int pos_in_n = letter_start_in_n + pos;

        if (parameters.verbosity > 3) { printf("\t\t\tpos: %d\n", pos); }

        // if (!utils::is_match_valid(spans, j, i_str.length(), pos,
        // letter_start_in_n)) { continue; }

        if (utils::is_match_valid(spans, j, i_str.length(), pos,
                                  letter_start_in_n) &&
            (i == 0 || j == 0 || // TODO: is this check robust?
             (utils::is_pos_at_start(spans, pos_in_n) &&
              utils::is_letter_matched(eds_q, j - 1, q_matrix, i)) ||
             utils::is_prefix_matched(eds_q, q_matrix, pos, i, j))) {
          int end_idx = pos_in_n + i_str.length() - 1;

          q_matrix[i][end_idx] = true;
          q_end_indexes.push_back(end_idx);

          inc_i = true;

          if (parameters.verbosity > 4) { printf("\t\t\tset i %lu \tpos_in_M: %d\n", i, end_idx); }

          std::set<int> myset;
          for (auto sp : spans) { myset.insert(sp.stop); }

          if (myset.find(end_idx) == myset.end()) {
            // found an implicit node
            

            for (int e =0 ; e < spans.size(); e++) {
              if (spans[e].stop >= end_idx) {
                int u = (end_idx - spans[e].start) + 1;
                // std::cerr << "\t\t\tImplicit match at: " << pos << "saving: " << e << " , "<< u << std::endl;
                next_j_valid_suffixes.push_back( std::make_pair(e,  u));
                break;
              }
            }
          }

          int y = eds_w.str_offsets[i][suff.first].stop;
          w_matrix[j][y] = true;
        }
      }
    }

    // -------
    // match in Q
    // -------

    // w_end_indexes.clear();
    root_text = w_suffix_trees[i];
    if (parameters.verbosity > 3) { printf("\ti text: %s\n", root_text.second.c_str()); }

    for (std::string str : eds_w.data[i].data) {
      std::cerr << str << ".";
    }
    if (eds_w.data[i].has_epsilon) { std::cerr << "."; }
    std::cerr << std::endl;

    // handle epsilon
    if (eds_q.data[j].has_epsilon &&
        (i == 0 || utils::is_letter_matched(eds_q, j - 1, q_matrix, i))) {
      std::vector<span> letter_span = eds_q.str_offsets[j];
      int epsilon_idx = letter_span.back().stop;

      q_matrix[i][epsilon_idx] = true;
      q_end_indexes.push_back(epsilon_idx);

      inc_j = true;
    }

    std::vector<std::string> j_strings = eds_q.data[j].data;

    for (auto suff : j_valid_suffixes) {

      // std::cerr << "fir " << suff.first << " sec " << suff.second << std::endl;
      // std::cerr << "full str " << eds_q.data[j].data[suff.first] << std::endl;


      std::string j_str = eds_q.data[j].data[suff.first].substr(suff.second);
    

        vector<int> res = FindEndIndexes(j_str.c_str(), &root_text.first,
                                         root_text.second.c_str());

        // if the string does not match anywhere
        if (res.empty()) {
          continue;
        }

        if (parameters.verbosity > 3) {
          printf("\t\tj_str: %s\n", j_str.c_str());
        }

        std::vector<span> spans = eds_w.str_offsets[i];
        int letter_start_in_n = spans.front().start;

        for (int pos : res) {

          if (parameters.verbosity > 3) {
            printf("\t\t\tpos: %d\n", pos);
          }

          // if (!utils::is_match_valid(spans, i, j_str.length(), pos,
          // letter_start_in_n)) { continue; }

          int pos_in_n = letter_start_in_n + pos;

          if (utils::is_match_valid(spans, i, j_str.length(), pos,
                                    letter_start_in_n) &&
              (j == 0 || i == 0 || // TODO: is this check robust enough?
               (utils::is_pos_at_start(spans, pos_in_n) &&
                utils::is_letter_matched(eds_w, i - 1, w_matrix,
                                         j)) || // TODO: what if i == 0?
               utils::is_prefix_matched(eds_w, w_matrix, pos, j, i))) {
            int end_idx = pos_in_n + j_str.length() - 1;

            w_matrix[j][end_idx] = true;
            w_end_indexes.push_back(end_idx);

            inc_j = true;

            if (parameters.verbosity > 4) {
              printf("\t\t\tset j %lu \tpos_in_n: %d\n", j, end_idx);
            }

            std::set<int> myset;
            for (auto sp : spans) { myset.insert(sp.stop); }

            if (myset.find(end_idx) == myset.end()) {
              // found an implicit node

              for (int d = 0; d < spans.size(); d++) {
                if (spans[d].stop >= end_idx) {
                  int u = (end_idx - spans[d].start) + 1;
                  // std::cerr << "\t\t\tImplicit match at: " << pos << "saving: " << d << " , " << u << std::endl;
                  next_i_valid_suffixes.push_back( std::make_pair(d, u));
                  break;
                }
              }
            }

            /*
            // found an implicit node
            for (auto sp : spans) {
              if (end_idx != sp.stop) {
                int internal_start = end_idx - spans.front().start;
                next_i_valid_suffixes.push_back(std::make_pair(suff.first,
            internal_start + 1));
              }
            }*/

            // std::cerr << "\t\tinc_j " << inc_j << std::endl;

            int y = eds_q.str_offsets[j][suff.first].stop;
            q_matrix[i][y] = true;
          }
        }
      }

    // if allowed, increment the row and copy over the previously matched rows
    auto inc_and_copy = [](matrix &dp_matrix, std::size_t *row, int max_row,
                           std::size_t cols) {
      ++(*row);
      if (*row < max_row) {
        for (int k=0; k < cols ; k++) {
          dp_matrix[*row][k] = dp_matrix[*row - 1][k];
        }
      }
    };

    // std::cerr << "\tinc i " << inc_i << "inc j " << inc_j << std::endl;

    if (inc_i) {
      inc_and_copy(q_matrix, &i, len_w, size_q);

      if (i < len_w) {
        i_valid_suffixes.clear();
        std::vector<span> i_offsets = eds_w.str_offsets[i];
        for (int str_idx = 0; str_idx < i_offsets.size(); str_idx++) {
          if (eds_w.data[i].has_epsilon && str_idx == i_offsets.size() - 1) {
            break;
          }
          int internal_start =
              i_offsets[str_idx].start - i_offsets.front().start;
          i_valid_suffixes.push_back(std::make_pair(str_idx, 0));
        }
      }

      
    } else {

      q_end_indexes.clear();

      for (auto n: next_i_valid_suffixes) {
        i_valid_suffixes.push_back(n);
      }
    }

    if (inc_j) {
      inc_and_copy(w_matrix, &j, len_q, size_w);

      if (j < len_q) {
        j_valid_suffixes.clear();
        std::vector<span> j_offsets = eds_q.str_offsets[j];
        for (int str_idx = 0; str_idx < j_offsets.size(); str_idx++) {
          if (eds_q.data[j].has_epsilon && str_idx == j_offsets.size() - 1) {
            break;
          }
          int internal_start =
              j_offsets[str_idx].start - j_offsets.front().start;
          j_valid_suffixes.push_back(std::make_pair(str_idx, 0));
        }
      }

      
    } else {
      w_end_indexes.clear();

      for (auto n : next_j_valid_suffixes) {
        j_valid_suffixes.push_back(n);
      }

    }

    // printf("-------------W----------\n");
    // print_matrix(w_matrix);
    // printf("-------------Q----------\n");
    // print_matrix(q_matrix);
    // printf("\n\n");

    // give up early because we can not extend any degenerate letter
    if (!inc_j && !inc_i) {
      if (parameters.verbosity > 2) { std::cerr << "INFO, [improved::intersect] quit early" << std::endl; }

      std::cerr << std::endl;
      utils::print_edt_range(eds_w, 0, i+1);
      std::cerr << std::endl;
      utils::print_edt_range(eds_q, 0, j+1);
      std::cerr << std::endl;

      return false;
    }
  }



  // computing accepting states
  std::vector<std::size_t> w_ends = utils::compute_str_ends(eds_w, len_w - 1);
  std::vector<std::size_t> q_ends = utils::compute_str_ends(eds_q, len_q - 1);

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

  // std::cout << "w: " << accept_w << " q: " << accept_q << std::endl;

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

bool intersect(EDS &eds_w, EDS &eds_q, core::Parameters parameters) {
  if (parameters.verbosity > 1) { printf("[main::naive::intersect]\n"); }

  LinearizedEDS linear_w = parser::linearize(eds_w);
  LinearizedEDS linear_q = parser::linearize(eds_q);

  size_t last_row = linear_w.str.length();
  size_t last_col = linear_q.str.length();

  boolean_matrix dp_matrix = utils::gen_matrix(last_row, last_col);
  // boolean_matrix q_dp_matrix = utils::gen_matrix(last_row, last_col);

  auto prev_matched = [&dp_matrix, &linear_w, &linear_q](int row,
                                                         int col) -> bool {
    std::vector<int> prev_w = linear_w.prev_chars[row];
    std::vector<int> prev_q = linear_q.prev_chars[col];

    // TODO: what if only one is empty?
    if (prev_w.empty() || prev_q.empty()) {
      return true;
    }

    // std::cout << row << " " << col << "\n";

    for (int prev_row_idx : prev_w) {
      for (int prev_col_idx : prev_q) {
        if (dp_matrix[prev_row_idx][prev_col_idx]) {
          return true;
        }
      }
    }

    return false;
  };

  auto chars_match = [&linear_w, &linear_q](int row_idx, int col_idx) -> bool {
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
          for (int r = 0; r <= row_idx; r++) {
            if (dp_matrix[r][c]) {
              dp_matrix[r][col_idx] = true;
            }
          }
        }
      }
    }
  }

  size_t_vec accept_q = utils::compute_accepting_states(eds_q);
  size_t_vec accept_w = utils::compute_accepting_states(eds_w);

  for (int row : accept_w) {
    for (int col : accept_q) {
      if (dp_matrix[row][col]) {
        return true;
      }
    }
  }

  return false;
}
}

