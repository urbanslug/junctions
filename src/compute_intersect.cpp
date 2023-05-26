#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <stdlib.h>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "./parser.hpp"
#include "./suffix_tree.cpp"
#include "core.hpp"
#include "utils.hpp"


namespace improved {

/**
 * if any of the offsets in the text_idx-1 is reachable from
 * i active suffixes or
 * i-1 full match
 *
 */
bool is_prev_letter_matched(int text_letter_idx, int query_letter_idx,
                            EDS const &query_eds, EDS const &text_eds,
                            matrix const &query_matrix,
                            matrix const &text_matrix,
                            core::Parameters const &parameters) {
  if (parameters.verbosity > 2) {
    std::cerr << utils::indent(1) << "DEBUG, [compute_intersect::prev_matched]"
              << std::endl;
  }

  // std::cerr << text_letter_idx << " " << query_letter_idx << std::endl;

  if ( query_letter_idx == 0) { return true; }
  if (text_letter_idx == 0 && query_letter_idx == 0) { return true; }

  // TODO: ?
  if (text_letter_idx == 1 && query_letter_idx == 0 && text_eds.data[0].has_epsilon) {
    return true;
  }

  if (query_letter_idx == 1 && text_letter_idx == 0 && query_eds.data[0].has_epsilon) {
    return true;
  }

  int q =  query_letter_idx - 1;

  for (int row = text_letter_idx; row >= 0 && row >= text_letter_idx - 1; row--){
    //if (query_letter_idx <= 0) { break; } // can move up nd out of loop
    for (auto col_span = query_eds.str_offsets[q].begin();
         col_span != query_eds.str_offsets[q].end();
         col_span++) {
      // std::cerr << utils::indent(2) << "[" << row << "][" << col_span->stop << "]: " << (query_matrix[row][col_span->stop]) << std::endl;
      if (query_matrix[row][col_span->stop]) { return true; }
    }
  }

  if (text_letter_idx == 0) {return false; }

  for (auto col_span = text_eds.str_offsets[text_letter_idx -1 ].begin();
       col_span != text_eds.str_offsets[text_letter_idx - 1].end();
       col_span++) {
    // std::cerr << utils::indent(2) << "[" << query_letter_idx << "][" << col_span->stop << "]: " << (text_matrix[query_letter_idx][col_span->stop]) << std::endl;
    if (text_matrix[query_letter_idx][col_span->stop]) { return true; };
  }

  return false;
}



/**
 *
 * Validate matches
 * find out where they start and end within the query and the text
 *
 * active suffixes exist in N
 *
 * txt_slices exist in l start and length
 * txt_offsets exist in N start and stop indexes of a str in N
 * qry_offsets exist in N start and stop indexes of a str in N
 * return
 */
void update_matrices(std::vector<junctions::match> const &candidate_matches,
                         std::vector<slicex> const &txt_slices,
                         std::vector<span> const &txt_offsets,
                         std::vector<span> const &qry_offsets,
                         matrix *t_matrix, matrix *q_matrix,
                         int qry_letter_idx, int txt_letter_idx,
                         core::Parameters const &parameters) {
  // assume letter is reachable
  if (parameters.verbosity > 2) {
    std::cerr << utils::indent(1) << "DEBUG, [compute_intersect::filter_matches]" << std::endl;
  }


  //int txt_start_in_N = txt_offsets[0].start;
  auto in_txt_N = [&](int k) -> int { return txt_offsets[0].start + k; };
  auto in_txt_N_2 = [&](int idx, int k) -> int { return txt_offsets[idx].start + k; };
  //auto in_qry_N = [&](int l) -> int { return qry_offsets[0].start + l; };

  // std::vector<junctions::graph_slice> v;

  for (auto candiate_match : candidate_matches) {

    if (parameters.verbosity > 3) {
      std::cerr << junctions::indent(2) << candiate_match << std::endl;
    }

    slicex txt_slice = txt_slices[candiate_match.text_str_index];

    /*
      candiate_match.text_char_index: the start of the position of text within the concatenated txt
     */

    // this is the start position of the char within the specific string in the txt set
    int match_start_in_txt = candiate_match.text_char_index -  txt_slice.start;

    /*
      evaluate the start of the match
      ===============================
     */

    // is the active suffix valid?
    bool valid_as = false;

    // TODO: rename t_start in N to something like match_start_in_N
    int t_start_in_N = in_txt_N(candiate_match.text_char_index);

    if (parameters.verbosity > 3) {
      std::cerr << utils::indent(2)
                << "txt_str_idx: " << candiate_match.text_str_index
                << " start char in concat l: " << candiate_match.text_char_index
                << " start char in l:  " << match_start_in_txt
                << " char start in N (not concat): " << t_start_in_N
                << std::endl;
    }

    // int txt_end = txt_slice.start + txt_slice.length;

    // the match starts within a string in the text
    // second condition because actv suff can only be extended ...
    if (match_start_in_txt > 0 && qry_letter_idx > 0) {
        // is valid active suffix
        // int in_N = in_txt_N(candiate_match.text_char_index); wrong
      // do we need to confirm t_start_in_N - 1
      valid_as = ((*t_matrix)[qry_letter_idx - 1][t_start_in_N - 1] == 1);
    }

    // is an exp - exp match
    // or valid active suffix
    // txt start is not valid so skip
    if (match_start_in_txt > 0 && !valid_as) { continue; }


    /*
      evaluate the END of the match
      ===============================
    */


    // where the match ends
    int candidate_match_end = match_start_in_txt + candiate_match.match_length;

    // where the matched string actually ends
    // int txt_slice_end = txt_slice.start + txt_slice.length;

    if (parameters.verbosity > 3) {
      std::cerr << utils::indent(2)
                << "txt_slice.start " << txt_slice.start
                << " candidate_match_end " << candidate_match_end
                << std::endl;
    }

    // mark the end of the match as reachable in N_1 and N_2

    // in the txt
    int in_N = in_txt_N_2(candiate_match.text_str_index, candidate_match_end);

    // std::cerr << utils::indent(2) << "save as: " << qry_letter_idx << in_N;
    if (parameters.verbosity > 3) {
      std::cerr << utils::indent(2)
                << "t set: [" << qry_letter_idx << "][" << (in_N - 1) << "]"
                << std::endl;
    }

    (*t_matrix)[qry_letter_idx][in_N-1] = 1;

    /*
      Handle query
      ============

      queries always start at 0 so we don't need to do any additional stuff
     */

    // determine match start and ends in N

    // in the query
    int q_start_in_N = qry_offsets[candiate_match.query_str_index].start;

    if (parameters.verbosity > 3) {
      std::cerr << utils::indent(2)
                << "q set: [" << txt_letter_idx << "]"
                << "["<< (q_start_in_N + candiate_match.match_length - 1) << "]" << std::endl;
    }

    // mark the qry as reachable
    (*q_matrix)[txt_letter_idx] [q_start_in_N + candiate_match.match_length - 1] = 1;
  }
}
/*
Is there an intersection between ED strings W and Q?

                  T_2
         0   ...    j   ...   n_2
         |--------------------|
       . |                    |
       . |                    |
       . |                    |
   T_1 . |                    |
       i |                    |
       . |                    |
       . |                    |
       . |                    |
         |--------------------|
         n_1
*/
bool intersect(EDS &eds_w, EDS &eds_q, core::Parameters parameters) {
  if (parameters.verbosity > 1) { std::cerr << "DEBUG, [improved::intersect]" << std::endl; }

  size_t size_w = eds_w.size;
  size_t size_q = eds_q.size;

  int len_w = eds_w.length;
  int len_q = eds_q.length;

  matrix w_matrix = utils::gen_matrix(len_q, size_w + eds_w.epsilons);
  matrix q_matrix = utils::gen_matrix(len_w, size_q + eds_q.epsilons);

  /*
    Generate suffix trees
    -----------------------
  */

  std::vector<std::pair<STvertex, std::string>> w_suffix_trees;
  w_suffix_trees.reserve(len_w);
  std::vector<std::pair<STvertex, std::string>> q_suffix_trees;
  q_suffix_trees.reserve(len_q);

  auto t0 = Time::now();
  std::chrono::duration<double> timeRefRead;

  if (parameters.verbosity > 5) {
    std::cerr << "DEBUG, [improved::intersect] Generating suffix trees" << std::endl;
  }


  
  gen_suffix_tree(eds_w, &w_suffix_trees);
  gen_suffix_tree(eds_q, &q_suffix_trees);

  // std::thread t1(gen_suffix_tree, std::ref(eds_w), len_w, &w_suffix_trees);
  // std::thread t2(gen_suffix_tree, std::ref(eds_q), len_q, &q_suffix_trees);
  // t1.join();
  // t2.join();

  timeRefRead = Time::now() - t0;

  if (parameters.verbosity > 5) {
    std::cerr
      << "DEBUG, [improved::intersect] Time spent generating suffix trees: "
      << timeRefRead.count() << " sec" << std::endl;
  }

  /*
    Find the intersection
    ---------------------
  */

  // TODO: remove
  std::vector<int> match_positions;
  // TODO: remove move this upwards
  // std::vector<match_data> matches_found; // how many could these be?
  // TODO: remove
  // std::vector<suffix> w_suffixes, q_suffixes;
  std::vector<junctions::match> candidate_matches;

  bool prev_j = false, prev_i = false;


  for (int i = 0; i < len_w; i++) {
    for (int j = 0; j < len_q; j++) {

      if (parameters.verbosity > 2) {
        std::cerr << utils::indent(1) << "i: " << i << " j: " << j << std::endl
                  << utils::indent(1) << "--------------" << std::endl;
      }

      // Handle ε
      if (i == 0 && j == 0 && eds_w.data[i].has_epsilon && eds_q.data[j].has_epsilon) {
        w_matrix[0][eds_w.str_offsets[0].back().stop] = true;
        q_matrix[0][eds_q.str_offsets[0].back().stop] = true;
      }

      if (eds_w.data[i].has_epsilon) {
        // W[i] is reachable

        // copy right
        if (i > 0) {
          for (span s : eds_w.str_offsets[i - 1]) {
            if (w_matrix[j][s.stop]) {
              int eps_col = eds_w.str_offsets[i].back().stop;
              w_matrix[j][eps_col] = true;
              break;
            }
          }
        }
        // copy down
        if (i > 0) {
          for (size_t col = eds_q.str_offsets[j].front().start; col <= eds_q.str_offsets[j].back().stop; col++) {

            if (q_matrix[i - 1][col]) { q_matrix[i][col] = true; }
          }
        }
      }

      if (eds_q.data[j].has_epsilon) {
        // copy right
        if (j > 0) {
          for(span s: eds_q.str_offsets[j-1]) {
            if (q_matrix[i][s.stop]) {
              int eps_col = eds_q.str_offsets[j].back().stop;
              q_matrix[i][eps_col] = true;
              break;
            }
          }
        }

        if (j > 0) {
          // copy down
          for (size_t col = eds_w.str_offsets[i].front().start; col <= eds_w.str_offsets[i].back().stop; col++) {
            if (w_matrix[j - 1][col]) { w_matrix[j][col] = true; }
          }
        }
      }

      prev_j = is_prev_letter_matched(i, j, eds_q, eds_w, q_matrix, w_matrix, parameters);
      prev_i = is_prev_letter_matched(j, i, eds_w, eds_q, w_matrix, q_matrix, parameters);

      if (!(prev_i && prev_j) ) { continue; }

      // search for j_strs in i and update matrices
      if (parameters.verbosity > 3) {
        std::cerr << utils::indent(1) << "DEBUG, "
                  << "Text => T" << junctions::unicode_sub_1 << "[" << i << "] "
                  << "Query => T" << junctions::unicode_sub_2 << "[" << j << "]"
                  << std::endl;
      }
      junctions::perform_matching(eds_q.data[j].data, eds_w.str_slices[i],
                                  &w_suffix_trees[i], &candidate_matches,
                                  parameters);

      update_matrices(candidate_matches, eds_w.str_slices[i],
                      eds_w.str_offsets[i], eds_q.str_offsets[j], &w_matrix,
                      &q_matrix, j, i, parameters);


      candidate_matches.clear();

      // search for i_strs in j
      if (parameters.verbosity > 3) {

        std::cerr << utils::indent(1) << "DEBUG, "
                  << "Text => T" << junctions::unicode_sub_2 << "[" << j << "] "
                  << "Query => T" << junctions::unicode_sub_1 << "[" << i << "]"
                  << std::endl;
      }

      junctions::perform_matching(eds_w.data[i].data, eds_q.str_slices[j], &q_suffix_trees[j],
                                  &candidate_matches, parameters);

      update_matrices(candidate_matches, eds_q.str_slices[j],
                      eds_q.str_offsets[j], eds_w.str_offsets[i],
                      &q_matrix, &w_matrix, i, j, parameters);

      candidate_matches.clear();
    }
  }


  size_t_vec q_cols = utils::compute_accepting_states(eds_q);
  size_t_vec w_cols = utils::compute_accepting_states(eds_w);

  bool accept_w, accept_q = false;
  for (size_t col : q_cols) {
    if (q_matrix[len_w - 1][col]) { accept_q = true; break; }
  }

  for (size_t col : w_cols) {
    if (w_matrix[len_q - 1][col]) { accept_w = true; break; }
  }

  if (false) {
    printf("w_matrix \n");
    utils::print_matrix(w_matrix);
    printf("\nq_matrix\n");
    utils::print_matrix(q_matrix);
  }

  return accept_q && accept_w;
}

}

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

bool intersect(EDS &eds_w, EDS &eds_q, core::Parameters parameters) {
  if (parameters.verbosity > 1) { std::cerr << "DEBUG, [naive::intersect]" << std::endl; }

  LinearizedEDS linear_w = parser::linearize(eds_w);
  LinearizedEDS linear_q = parser::linearize(eds_q);

  size_t last_row = linear_w.str.length();
  size_t last_col = linear_q.str.length();

  boolean_matrix dp_matrix = utils::gen_matrix(last_row, last_col);
  // boolean_matrix q_dp_matrix = utils::gen_matrix(last_row, last_col);

  // TODO: pass row and col as size_t
  auto prev_matched =
    [&dp_matrix, &linear_w, &linear_q, &eds_w, &eds_q](int row, int col) -> bool {
    // std::cout << row << " " << col << "\n";
    std::vector<int> prev_w = linear_w.prev_chars[row];
    std::vector<int> prev_q = linear_q.prev_chars[col];

    // only happens at letter 0
    // TODO: what if only one is empty?
    if (prev_w.empty() || prev_q.empty()) {
      // in the same letter

      if (eds_q.str_offsets[0].back().stop >= (size_t)col && eds_w.str_offsets[0].back().stop >= (size_t)row) {
        if (prev_q.empty() && prev_w.empty()) {
          // std::cout << "j" << std::endl;
          return true;
        }
      }

      // start has epsilon
      if (prev_q.empty() && !prev_w.empty()) {
        for (int prev_row_idx : prev_w) {
          if (linear_w.str[prev_row_idx] == '*') {
            //std::cout << "a" << std::endl;
            return true;
          }
        }
      }

      if (!prev_q.empty() && prev_w.empty()) {
        for (int prev_col_idx : prev_q) {
          if (linear_q.str[prev_col_idx] == '*') {
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
    return linear_w.str[row_idx] == linear_q.str[col_idx];
  };


  for (size_t row_idx = 0; row_idx < last_row; row_idx++) {
    for (size_t col_idx = 0; col_idx < last_col; col_idx++) {

      if (chars_match(row_idx, col_idx) && prev_matched(row_idx, col_idx)) {
        dp_matrix[row_idx][col_idx] = true;
      }

      if (linear_w.str[row_idx] == '*' && col_idx == last_col - 1) {
        int_vec prev_w = linear_w.prev_chars[row_idx];

        for (int r : prev_w) {
          for (size_t c = 0; c < last_col; c++) {
            if (dp_matrix[r][c]) {
              dp_matrix[row_idx][c] = true;
            }
          }
        }
      }

      if (linear_q.str[col_idx] == '*') {
        int_vec prev_q = linear_q.prev_chars[col_idx];

        for (int c : prev_q) {
          if (dp_matrix[row_idx][c]) {
            dp_matrix[row_idx][col_idx] = true;
          }
          /*
          for (int r = 0; r <= row_idx; r++) {
            if (dp_matrix[r][c]) {
              dp_matrix[r][col_idx] = true;
            }
          }
          */
        }
      }
    }
  }

  if (false) {
    printf(" naive \n");
    utils::print_matrix(dp_matrix);
    printf("\n\n");
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
