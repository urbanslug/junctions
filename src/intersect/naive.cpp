
#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <set>
#include <stdlib.h>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "./intersect.hpp"

namespace intersect {
namespace naive {

/*
                 Q
        0       ...          N
        |--------------------|
        |                    |
        |                    |
      . |                    |
    W . |                    |
      . |                    |
        |                    |
        |                    |
        |                    |
        |--------------------|
        M
*/

bool has_intersection(eds::EDS &eds_w, eds::EDS &eds_q) {
  eds_w.linearize();
  eds_q.linearize();


  eds::EDS& linear_w = eds_w;
  eds::EDS &linear_q = eds_q;

  size_t last_row = eds_w.get_size() + eds_w.get_eps_count();
  size_t last_col = eds_q.get_size() + eds_q.get_eps_count();

  n_core::bool_matrix dp_matrix = n_core::gen_matrix(last_row, last_col);
  
  // TODO: pass row and col as size_t
  auto prev_matched =
    [&dp_matrix, &linear_w, &linear_q, &eds_w, &eds_q](int row, int col) -> bool {
    // std::cout << row << " " << col << "\n";
    std::vector<std::size_t>& prev_w =  linear_w.get_prev_char_idx(row);
    std::vector<std::size_t>& prev_q =  linear_q.get_prev_char_idx(col);
    //std::vector<int> prev_w = linear_w.prev_chars[row];
    //std::vector<int> prev_q = linear_q.prev_chars[col];

    // only happens at letter 0
    // TODO: what if only one is empty?
    if (prev_w.empty() || prev_q.empty()) {
      // in the same letter

      if (eds_q.get_letter_boundaries(0).second >= (size_t)col &&
          eds_w.get_letter_boundaries(0).second >= (size_t)row) {
        if (prev_q.empty() && prev_w.empty()) {
          // std::cout << "j" << std::endl;
          return true;
        }
      }

      // start has epsilon
      if (prev_q.empty() && !prev_w.empty()) {
        for (int prev_row_idx : prev_w) {
          if (linear_w.get_char_at(prev_row_idx) == '*') {
            //std::cout << "a" << std::endl;
            return true;
          }
        }
      }

      if (!prev_q.empty() && prev_w.empty()) {
        for (int prev_col_idx : prev_q) {
          if (linear_q.get_char_at(prev_col_idx) == '*') {
            //std::cout << "b" << std::endl;
            return true;
          }
        }
      }
    }

    for (int prev_row_idx : prev_w) {
      for (int prev_col_idx : prev_q) {
        if (dp_matrix[prev_row_idx][prev_col_idx]) {
          // std::cout << "c" << std::endl;
          return true;
        }
      }
    }

    return false;
  };

  auto chars_match = [&linear_w, &linear_q](int row_idx, int col_idx) -> bool {
    return linear_w.get_char_at(row_idx) == linear_q.get_char_at(col_idx);
  };


  for (size_t row_idx = 0; row_idx < last_row; row_idx++) {
    for (size_t col_idx = 0; col_idx < last_col; col_idx++) {

      if (chars_match(row_idx, col_idx) && prev_matched(row_idx, col_idx)) {
        dp_matrix[row_idx][col_idx] = true;
      }

      if (linear_w.get_char_at(row_idx) == '*' && col_idx == last_col - 1) {
        std::vector<std::size_t> &prev_w = linear_w.get_prev_char_idx(row_idx);
        // int_vec prev_w = linear_w.prev_chars[row_idx];

        for (int r : prev_w) {
          for (size_t c = 0; c < last_col; c++) {
            if (dp_matrix[r][c]) {
              dp_matrix[row_idx][c] = true;
            }
          }
        }
      }

      // TODO: ??
      if (linear_q.get_char_at(col_idx) == '*') {
        std::vector<std::size_t> &prev_q = linear_q.get_prev_char_idx(col_idx);
        //int_vec prev_q = linear_q.prev_chars[col_idx];

        for (int c : prev_q) {
          if (dp_matrix[row_idx][c]) {
            dp_matrix[row_idx][col_idx] = true;
          }
        }
      }
    }
  }


  std::vector<std::size_t> accept_q =
      eds_q.get_letter_ends_global(eds_q.get_length() - 1);
  std::vector<std::size_t> accept_w =
      eds_w.get_letter_ends_global(eds_w.get_length() - 1);



  // size_t_vec accept_q = utils::compute_accepting_states(eds_q);
  //size_t_vec accept_w = utils::compute_accepting_states(eds_w);

  for (int row : accept_w) {
    for (int col : accept_q) {
      if (dp_matrix[row][col]) { return true; }
    }
  }

  return false;



}
}
}
