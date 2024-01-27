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


namespace intersect::naive {

/*
                 T2
        0       ...          N
        |--------------------|
        |                    |
        |                    |
      . |                    |
   T1 . |                    |
      . |                    |
        |                    |
        |                    |
        |                    |
        |--------------------|
        M
*/
bool has_intersection(eds::EDS &eds_t1, eds::EDS &eds_t2) {
  eds_t1.linearize();
  eds_t2.linearize();

  eds::EDS& linear_t1 = eds_t1;
  eds::EDS& linear_t2 = eds_t2;

  size_t last_row = eds_t1.get_size() + eds_t1.get_eps_count();
  size_t last_col = eds_t2.get_size() + eds_t2.get_eps_count();

  core::bool_matrix dp_matrix = core ::gen_matrix(last_row, last_col);

  auto prev_matched = [&dp_matrix, &linear_t1, &linear_t2, &eds_t1, &eds_t2]
    (std::size_t row, std::size_t col) -> bool {
    // std::cout << row << " " << col << "\n";
    std::vector<std::size_t>& prev_t1 = linear_t1.get_prev_char_idx(row);
    std::vector<std::size_t>& prev_t2 = linear_t2.get_prev_char_idx(col);

    // only happens at letter 0
    if (prev_t1.empty() || prev_t2.empty()) {
      // in the same letter

      if (eds_t2.get_letter_boundaries(0).right() >= col &&
          eds_t1.get_letter_boundaries(0).right() >= row) {
        if (prev_t2.empty() && prev_t1.empty()) {
          return true;
        }
      }

      // start has epsilon
      if (prev_t2.empty() && !prev_t1.empty()) {
        bool v {true};
        std::size_t l{};
        for (std::size_t ltr_idx{}; ltr_idx < eds_t1.get_length(); ltr_idx++) {
          auto [_, r] = eds_t1.get_letter_boundaries(ltr_idx);
          if (r < row) {
            l = ltr_idx;
            v = v && eds_t1.is_letter_eps(ltr_idx);
          }
          else {
            break;
          }
        }

        if (l==0) {
          return false;
        }

        if (v) { return true; }
      }

      if (!prev_t2.empty() && prev_t1.empty()) {
        bool v {true};
        std::size_t l{};
        for (std::size_t ltr_idx{}; ltr_idx < eds_t2.get_length(); ltr_idx++) {
          auto [_, r] = eds_t2.get_letter_boundaries(ltr_idx);
          if (r < col) {
            l = ltr_idx;
            v = v && eds_t2.is_letter_eps(ltr_idx);
          }
          else {
            break;
          }
        }

        if (l==0) {
          return false;
        }

        if (v) { return true; }
      }
    }

    for (std::size_t prev_row_idx : prev_t1) {
      for (std::size_t prev_col_idx : prev_t2) {
        if (dp_matrix[prev_row_idx][prev_col_idx]) {
          return true;
        }
      }
    }

    return false;
  };

  // do the chars at row_idx and col_idx match?
  auto chars_match = [&linear_t1, &linear_t2](std::size_t row_idx, std::size_t col_idx) -> bool {
    return linear_t1.get_char_at(row_idx) == linear_t2.get_char_at(col_idx);
  };

  for (size_t row_idx{}; row_idx < last_row; row_idx++) {
    for (size_t col_idx{}; col_idx < last_col; col_idx++) {

      if (chars_match(row_idx, col_idx) && prev_matched(row_idx, col_idx)) {
        dp_matrix[row_idx][col_idx] = true;
      }

      if (linear_t1.get_char_at(row_idx) == '*' && col_idx == last_col - 1) {
        std::vector<std::size_t> &prev_t1 = linear_t1.get_prev_char_idx(row_idx);

        for (std::size_t r : prev_t1) {
          for (size_t c{}; c < last_col; c++) {
            if (dp_matrix[r][c]) {
              dp_matrix[row_idx][c] = true;
            }
          }
        }
      }

      if (linear_t2.get_char_at(col_idx) == '*') {
        std::vector<std::size_t> &prev_t2 = linear_t2.get_prev_char_idx(col_idx);

        for (std::size_t c : prev_t2) {
          if (dp_matrix[row_idx][c]) {
            dp_matrix[row_idx][col_idx] = true;
          }
        }
      }
    }
  }

  std::vector<std::size_t> accept_t2 = eds_t2.get_letter_ends_global(eds_t2.get_length() - 1);
  std::vector<std::size_t> accept_t1 = eds_t1.get_letter_ends_global(eds_t1.get_length() - 1);

#ifdef DEBUG
  for (auto row : dp_matrix) {
    for (auto col : row) {
      std::cerr << col << " ";
    }
    std::cerr << std::endl;
  }
#endif

  for (std::size_t row : accept_t1) {
    for (std::size_t col : accept_t2) {
      if (dp_matrix[row][col]) { return true; }
    }
  }

  return false;
}
} // namespace intersect::naive
