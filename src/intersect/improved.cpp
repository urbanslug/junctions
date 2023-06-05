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

#include "./intersect.hpp"

namespace intersect {
namespace improved {

/**
 * if any of the offsets in the text_idx-1 is reachable from
 * i active suffixes or
 * i-1 full match
 *
 */
bool is_prev_letter_matched(int text_letter_idx, int query_letter_idx,
                            eds::EDS& query_eds, eds::EDS& text_eds,
                            n_core::bool_matrix const &query_matrix,
                            n_core::bool_matrix const &text_matrix) {
  if ( query_letter_idx == 0) { return true; }
  if (text_letter_idx == 0 && query_letter_idx == 0) { return true; }

  // TODO: ?

  if (text_letter_idx == 1 && query_letter_idx == 0 && text_eds.is_letter_eps(0)) {
    return true;
  }

  if (query_letter_idx == 1 && text_letter_idx == 0 && query_eds.is_letter_eps(0)) {
    return true;
  }

  int q =  query_letter_idx - 1;

  for (int row = text_letter_idx; row >= 0 && row >= text_letter_idx - 1; row--){
    for (auto e: query_eds.get_letter_ends_global(q)) {
      if (query_matrix[row][e]) { return true; }
    }
  }

  if (text_letter_idx == 0) {return false; }

  for (auto e: text_eds.get_letter_ends_global(text_letter_idx -1)) {
    if (text_matrix[query_letter_idx][e]) { return true; };
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
void update_matrices(std::vector<n_junctions::match> const &candidate_matches,
                     eds::EDS &txt_eds,
                     std::vector<eds::slice_eds> const &txt_offsets,
                     std::vector<eds::slice_eds> const &qry_offsets,
                     n_core::bool_matrix *t_matrix,
                     n_core::bool_matrix *q_matrix,
                     int qry_letter_idx,
                     int txt_letter_idx) {
  auto in_txt_N = [&](int k) -> int { return txt_offsets[0].start + k; };
  auto in_txt_N_2 = [&](int idx, int k) -> int { return txt_offsets[idx].start + k; };

  for (auto candiate_match : candidate_matches) {

    //slicex txt_slice = txt_slices[candiate_match.text_str_index];
    eds::slice_eds txt_slice =
      txt_eds.get_str_slice_local(txt_letter_idx, candiate_match.text_str_index);

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


    // int txt_end = txt_slice.start + txt_slice.length;

    // the match starts within a string in the text
    // second condition because actv suff can only be extended ...
    if (match_start_in_txt > 0 && qry_letter_idx > 0) {
        // is valid active suffix
        // int in_N = in_txt_N(candiate_match.text_char_index); wrong
      // do we need to confirm t_start_in_N - 1
      valid_as = ((*t_matrix)[qry_letter_idx - 1][t_start_in_N - 1] == 1);
      int idx { 1 };
      int pos{};
      bool shorter_match{false};

      for (int idx{1}; !valid_as && idx < candiate_match.match_length; idx++) {
        if ((*t_matrix)[qry_letter_idx - 1][t_start_in_N - 1 + idx]) {
          pos = idx;
          shorter_match = true;
          break;
        }
      }

      /*
      for (; (pos = (*t_matrix)[qry_letter_idx - 1][t_start_in_N - 1 + idx]) &&
             idx < candiate_match.match_length;
           idx++){}
      */

      if (!valid_as && shorter_match) {
        t_start_in_N += pos;
        match_start_in_txt += pos;
        candiate_match.text_char_index += pos;
        candiate_match.match_length -= pos;
        candiate_match.str = candiate_match.str.substr(pos);
        valid_as = true;
      }
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


    // mark the end of the match as reachable in N_1 and N_2

    // in the txt
    int in_N = in_txt_N_2(candiate_match.text_str_index, candidate_match_end);

    // std::cerr << utils::indent(2) << "save as: " << qry_letter_idx << in_N;

    (*t_matrix)[qry_letter_idx][in_N-1] = 1;

    /*
      Handle query
      ============

      queries always start at 0 so we don't need to do any additional stuff
     */

    // determine match start and ends in N

    if (match_start_in_txt == 0 &&
        txt_offsets[candiate_match.text_char_index].length == candiate_match.match_length &&
        !candiate_match.beyond_txt
        ){
      //std::cout << txt_letter_idx << " " << qry_letter_idx << "\n" ;
    }

    // in the query
         int q_start_in_N = qry_offsets[candiate_match.query_str_index].start;

     // mark the qry as reachable
     (*q_matrix)[txt_letter_idx]
                [q_start_in_N + candiate_match.match_length - 1] = 1;
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
bool has_intersection(eds::EDS &eds_w, eds::EDS &eds_q) {
  
  size_t size_w = eds_w.get_size();
  size_t size_q = eds_q.get_size();

  int len_w = eds_w.get_length();
  int len_q = eds_q.get_length();

  n_core::bool_matrix w_matrix = n_core::gen_matrix(len_q, size_w + eds_w.get_eps_count());
  n_core::bool_matrix q_matrix = n_core::gen_matrix(len_w, size_q + eds_q.get_eps_count());

  /*
    Generate suffix trees
    -----------------------
  */

  std::vector<std::pair<match_st::STvertex, std::string>> w_suffix_trees;
  w_suffix_trees.reserve(len_w);
  std::vector<std::pair<match_st::STvertex, std::string>> q_suffix_trees;
  q_suffix_trees.reserve(len_q);

  auto t0 = n_core::Time::now();
  std::chrono::duration<double> timeRefRead;

  gen_suffix_tree(eds_w, &w_suffix_trees);
  gen_suffix_tree(eds_q, &q_suffix_trees);

  // std::thread t1(gen_suffix_tree, std::ref(eds_w), len_w, &w_suffix_trees);
  // std::thread t2(gen_suffix_tree, std::ref(eds_q), len_q, &q_suffix_trees);
  // t1.join();
  // t2.join();

  timeRefRead = n_core::Time::now() - t0;

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
  std::vector<n_junctions::match> candidate_matches;

  bool prev_j = false, prev_i = false;

  // TODO: make these ints size_t
  for (int i = 0; i < len_w; i++) {
    for (int j = 0; j < len_q; j++) {


      // Handle ε
      if (i == 0 && j == 0 && eds_w.is_letter_eps(i) && eds_q.is_letter_eps(j)) {
        w_matrix[0][eds_w.get_global_eps_idx(i)] = true;
        q_matrix[0][eds_q.get_global_eps_idx(j)] = true;
        //w_matrix[0][eds_w.str_offsets[0].back().stop] = true;
        //q_matrix[0][eds_q.str_offsets[0].back().stop] = true;
      }

      if (eds_w.is_letter_eps(i)) {
        // W[i] is reachable

        // copy right
        /// TODO: check once if i is greater than 0 with is letter eps
        if (i > 0) {

          for (std::size_t s : eds_w.get_letter_ends_global(i - 1)) {
            if (w_matrix[j][s]) {
              w_matrix[j][eds_w.get_global_eps_idx(i)] = true;
              break;
            }
          }
        }

        if (i > 0) {
          // copy down
          std::pair<std::size_t, std::size_t> b = eds_q.get_letter_boundaries(j);
          for (std::size_t col = b.first; col <= b.second; col++) {
            if (q_matrix[i - 1][col]) { q_matrix[i][col] = true; }
          }
        }
      }

      if (eds_q.is_letter_eps(j)) {
        // copy right
        if (j > 0) {
          for(std::size_t s : eds_q.get_letter_ends_global(j - 1)) {
            if (q_matrix[i][s]) {
              q_matrix[i][eds_q.get_global_eps_idx(j)] = true;
              break;
            }
          }
        }

        if (j > 0) {
          // copy down
          std::pair<std::size_t, std::size_t> b = eds_w.get_letter_boundaries(i); // dry with other copy down
          for (std::size_t col = b.first; col <= b.second; col++) {
            if (w_matrix[j - 1][col]) { w_matrix[j][col] = true; }
          }
        }
      }

      prev_j = is_prev_letter_matched(i, j, eds_q, eds_w, q_matrix, w_matrix);
      prev_i = is_prev_letter_matched(j, i, eds_w, eds_q, w_matrix, q_matrix);

      if (!(prev_i && prev_j) ) {
        continue;
      }


      // Search for j_strs in T_1[i] and update matrices
      // ----------------------------------------------

      n_junctions::perform_matching(
          eds_q.get_strs(j),
          &w_suffix_trees[i],
          &candidate_matches);

      update_matrices(candidate_matches,
                      eds_w,
                      eds_w.get_slice(i),
                      eds_q.get_slice(j),
                      &w_matrix, &q_matrix,
                      j, i);

      candidate_matches.clear();

      // Search for i_strs in T_2[j] and update matrices
      // -----------------------------------------------



      n_junctions::perform_matching(
          eds_w.get_strs(i),
          &q_suffix_trees[j],
          &candidate_matches);


      update_matrices(candidate_matches,
                      eds_q,
                      eds_q.get_slice(j),
                      eds_w.get_slice(i),
                      &q_matrix, &w_matrix,
                      i, j);

      candidate_matches.clear();
    }
  }

  

    std::vector<std::size_t> q_cols =
        eds_q.get_letter_ends_global(eds_q.get_length() - 1);
    std::vector<std::size_t> w_cols =
        eds_w.get_letter_ends_global(eds_w.get_length() - 1);

    bool accept_w, accept_q = false;
    for (size_t col : q_cols) {
    if (q_matrix[len_w - 1][col]) { accept_q = true; break; }
  }

  for (size_t col : w_cols) {
    if (w_matrix[len_q - 1][col]) { accept_w = true; break; }
  }

  return accept_q && accept_w;
}

}
}
