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
#include <bitset>


#include "./intersect.hpp"

namespace intersect::improved {

/**

   Checks whether a set i or j is reachable from another j or i in the
   intersection graph considering only (imp,exp) or (exp,imp) vertices in the
   graph starting from (0,0).

   Given a pair of sets (i,j).

   We return true (say that prev_i matched) if one of the below cases is true.

   from i

   Case 1: i > 0 && j > 0
   ======================
   to be true, one of the following must be true:
      a) exp, exp
          one of the strings in i-1 matched to the end of j-1
      b) imp, exp
          one of the strings in i matched to the end of j-1
          creating an active suffix in i

   Case 2: i > 0 && j == 0
   =======================
   to be true, i-1 matched to the end (exp) with j (imp)
   creating an active suffix in j

   Case 3: i == 0 && j > 0
   =======================
   to be true, i matched partially (imp) with j-1 (exp)
   creating an active suffix in i

   from j

   case 2: i == 0 && j > 0
   =======================

   case 3: i > 0 && j == 0
   =======================

 */
bool exp_exp(std::size_t i, std::size_t j,
             eds::EDS& eds_t1, eds::EDS& eds_t2,
             core::bool_matrix const &t1, core::bool_matrix const &t2) {
  if (i == 0 && j == 0) { return true; }

  if (i > 0 && j > 0) {

    bool exp_1{false}, exp_2{false};

    for (auto x : eds_t2.get_slice(j-1)) {
      std::size_t l = x.length > 0 ? x.length - 1 : 0;
      if (t2[i-1][x.start + l]) { exp_1 = true; break; }
    }

    for (auto x : eds_t1.get_slice(i-1)) {
      std::size_t l = x.length > 0 ? x.length - 1 : 0;
      if (t1[j-1][x.start + l]) { exp_2 = true; break; }
    }

    return exp_1 && exp_2;
  }
  else if (i == 0 && j > 0) {
    for (std::size_t j_{}; j_ < j; j_++) {
      if (!eds_t2.is_letter_eps(j_)) { return false; }
    }

    for (auto x : eds_t1.get_slice(0)) {
      if (t1[j-1][x.start + x.length -1]) { return true; }
    }
  }
  else if (i > 0 && j == 0) {
    for (std::size_t i_{}; i_ < i; i_++) {
      if (!eds_t1.is_letter_eps(i_)) { return false; }
    }

    for (auto x : eds_t2.get_slice(0)) {
      if (t2[i-1][x.start + x.length -1]) { return true; }
    }
  }

  return false;
}

bool imp_exp(std::size_t i, std::size_t j,  eds::EDS& eds_t1, core::bool_matrix const &t1) {

  if (j == 0) { return false;}

  std::set<std::size_t> str_ends;
  for (auto x : eds_t1.get_slice(i)) {
    if (!x.eps_slice){ str_ends.insert(x.start +x.length -1 ); }
  }

  std::size_t col = eds_t1.get_letter_boundaries(i).left();
  std::size_t end = eds_t1.get_letter_boundaries(i).right();
  for (; col <= end; col++) {
    if (!str_ends.count(col) && t1[j - 1][col]) {return true; }
  }

  return false;
}

bool exp_imp(std::size_t i, std::size_t j, eds::EDS& eds_t2, core::bool_matrix const &t2) {

  if (i == 0) { return false; }

  // to be implicit you must match only internally
  std::set<std::size_t> str_ends;
  for (auto x : eds_t2.get_slice(j)) {
    if (!x.eps_slice){ str_ends.insert(x.start +x.length -1 ); }
  }

  std::size_t col = eds_t2.get_letter_boundaries(j).left();
  std::size_t end = eds_t2.get_letter_boundaries(j).right();
  for (; col <= end; col++) {
    if (!str_ends.count(col) && t2[i - 1][col]) {return true; }
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
void update_matrices(std::vector<core::EDSMatch> const &candidate_matches,
                     eds::EDS &txt_eds,
                     std::vector<eds::slice_eds> const &qry_offsets,
                     core::bool_matrix *t_matrix,
                     core::bool_matrix *t2_matrix,
                     std::size_t qry_letter_idx,
                     std::size_t txt_letter_idx,
                     std::tuple<std::bitset<2>, std::bitset<2>, std::bitset<2>> reachability) {
  for (core::EDSMatch candiate_match : candidate_matches) {
    // matches always start at the beginning of a query so no need to check for that
    if (candiate_match.get_char_idx() == 0 && std::get<2>(reachability).to_ulong() < 3) {
      continue;
    }

    eds::slice_eds local_txt_slice =
      txt_eds.get_str_slice_local(txt_letter_idx, candiate_match.get_txt_str_idx());

    /*
      evaluate the start of the match
      ===============================
    */

    bool valid_as{false}, // is the active suffix valid?
      text_stop_exp{false}, // is the end of the text reached?
      query_stop_exp{false}; // is the end of the query reached?

    std::size_t t_start_in_N =
      txt_eds.to_global_idx(txt_letter_idx, candiate_match.get_char_idx()) +
      local_txt_slice.start;

    // the match starts within a string in the text
    // second condition because actv suff can only be extended ...
    if (candiate_match.get_char_idx() > 0 && qry_letter_idx > 0) {
      // is valid active suffix
      // do we need to confirm t_start_in_N - 1
      valid_as = ((*t_matrix)[qry_letter_idx - 1][t_start_in_N - 1] == 1);
    }

    // is an exp - exp match
    // or valid active suffix
    // txt start is not valid so skip
    if (candiate_match.get_char_idx() > 0 && !valid_as) { continue; }


    /*
      evaluate the END of the match
      ===============================
    */

    // mark the end of the match as reachable in N_1 and N_2
    // match end in N

    // where the match ends locally
    std::size_t candidate_match_end =
      local_txt_slice.start + candiate_match.get_char_idx() + candiate_match.get_match_length();

    // where the matched string actually ends in l/k (locally)
    std::size_t txt_slice_end = local_txt_slice.start + local_txt_slice.length;
    if (candidate_match_end >= txt_slice_end) { text_stop_exp = true; }

    std::size_t t_end_in_N = t_start_in_N + candiate_match.get_match_length();

    /*
      Handle query
      ============

      queries always start at 0 so we don't need to do any additional stuff
    */

    // Determine match start and ends in N in the query EDS
    int q_start_in_N = qry_offsets[candiate_match.query_str_index].start;
    std::size_t qlen = qry_offsets[candiate_match.query_str_index].length;

    if (!candiate_match.is_beyond_txt() || !(candiate_match.get_match_length() < qlen)) {
      query_stop_exp = true;
    }

    // update matrices
    // ==================

    if (query_stop_exp) { (*t_matrix)[qry_letter_idx][t_end_in_N-1] = 1; }

    if (text_stop_exp) { // mark the qry as reachable
      (*t2_matrix)[txt_letter_idx][q_start_in_N + candiate_match.get_match_length() - 1] = 1;
    }
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
bool has_intersection(eds::EDS &eds_t1, eds::EDS &eds_t2) {

  size_t size_t1 = eds_t1.get_size();
  size_t size_t2 = eds_t2.get_size();

  std::size_t len_t1 = eds_t1.get_length();
  std::size_t len_t2 = eds_t2.get_length();

  core::bool_matrix t1_matrix = core::gen_matrix(len_t2, size_t1 + eds_t1.get_eps_count());
  core::bool_matrix t2_matrix = core::gen_matrix(len_t1, size_t2 + eds_t2.get_eps_count());

  /*
    Generate suffix trees
    -----------------------
  */

  std::vector<match_st::meta_st> w_suffix_trees;
  w_suffix_trees.reserve(len_t1);
  std::vector<match_st::meta_st> q_suffix_trees;
  q_suffix_trees.reserve(len_t2);

  auto t0 = core::Time::now();
  std::chrono::duration<double> timeRefRead;

  gen_suffix_tree_(eds_t1, &w_suffix_trees);
  gen_suffix_tree_(eds_t2, &q_suffix_trees);

  timeRefRead = core::Time::now() - t0;

  /*
    Find the intersection
    ---------------------
  */

  std::vector<core::EDSMatch> candidate_matches;

  // TODO: make these ints size_t
  for (std::size_t i{}; i < len_t1; i++) {
    for (std::size_t j{}; j < len_t2; j++) {

      /*
        Handle ε
        .........
       */

      if (i == 0 && j == 0) { // both are ε
        if (eds_t1.is_letter_eps(0)) {
          for (auto sl : eds_t2.get_slice(0)) {
            std::size_t str_end = sl.start + sl.length - 1;
            t2_matrix[0][str_end] = true;
          }
        }

        if (eds_t2.is_letter_eps(0)) {
          for (auto sl : eds_t1.get_slice(0)) {
            std::size_t str_end = sl.start + sl.length - 1;
            t1_matrix[0][str_end] = true;
          }
        }
      }

      if (eds_t1.is_letter_eps(i)) {
        if (i > 0) {
          // copy right
          // ----------

          for (std::size_t s : eds_t1.get_letter_ends_global(i - 1)) {
            if (t1_matrix[j][s]) {
              t1_matrix[j][eds_t1.get_global_eps_idx(i)] = true;
              break;
            }
          }
        }

        if (j==0 && i > 0) {
          // copy down
          // --------

          std::size_t col = eds_t2.get_letter_boundaries(j).left();

          // in case of active suffix
          std::size_t col_end = eds_t2.get_letter_boundaries(j).right();

          for (; col <= col_end; col++) {
            //continue;
            if (t2_matrix[i - 1][col]) { t2_matrix[i][col] = true; }
          }
        }

        if (j > 0 && i> 0) {
          // copy down
          // --------

          // TODO: combine with previous by starting with j or j-1 only when possible
          std::size_t col = eds_t2.get_letter_boundaries(j-1).left();

          // in case of active suffix
          std::size_t col_end = eds_t2.get_letter_boundaries(j).right();

          for (; col <= col_end; col++) {
            //continue;
            if (t2_matrix[i - 1][col]) { t2_matrix[i][col] = true; }
          }
        }
      }

      if (eds_t2.is_letter_eps(j)) {
        if (j > 0) {
          // copy right
          // ----------

          for(std::size_t s : eds_t2.get_letter_ends_global(j - 1)) {
            if (t2_matrix[i][s]) {
              t2_matrix[i][eds_t2.get_global_eps_idx(j)] = true;
              break;
            }
          }
        }


        if (i == 0 && j > 0) {
          // copy down
          // --------

          std::size_t col = eds_t1.get_letter_boundaries(i).left();

          // in case of active suffix
          std::size_t col_end = eds_t1.get_letter_boundaries(i).right();

          for (; col <= col_end; col++) {
            if (t1_matrix[j - 1][col]) { t1_matrix[j][col] = true; }
          }
        }

        if (i > 0 && j > 0) {
          // copy down
          // --------

          std::size_t col = eds_t1.get_letter_boundaries(i-1).left();

          // in case of active suffix
          std::size_t col_end = eds_t1.get_letter_boundaries(i).right();

          for (; col <= col_end; col++) {
            if (t1_matrix[j - 1][col]) { t1_matrix[j][col] = true; }
          }
        }
      }


      /*
        Test for reachability
        .....................
       */

      bool exp_exp_val = exp_exp(i, j, eds_t1, eds_t2, t1_matrix, t2_matrix);
      bool imp_exp_val = imp_exp(i, j, eds_t1, t1_matrix);
      bool exp_imp_val = exp_imp(i, j, eds_t2, t2_matrix);

#ifdef DEBUG

      //print i j values and their exp_exp_val values, etc.
      std::cout << "\n..............................\n";
        std::cout << "i: " << i << " j: " << j
                  << " exp_exp_val: " << exp_exp_val
                  << " imp_exp_val: " << imp_exp_val
                  << " exp_imp_val: " << exp_imp_val
                  << " res: " << (exp_exp_val || imp_exp_val || exp_imp_val)
                  << std::endl;
#endif

      // at least one of these must be true
      if (!(exp_exp_val || imp_exp_val || exp_imp_val) ) { continue; }

      std::bitset<2>  imp_exp = imp_exp_val ? std::bitset<2>("01") : std::bitset<2>("00");
      std::bitset<2>  exp_imp = exp_imp_val ? std::bitset<2>("10") : std::bitset<2>("00");
      std::bitset<2>  exp_exp = exp_exp_val ? std::bitset<2>("11") : std::bitset<2>("00");

      std::tuple<std::bitset<2>, std::bitset<2>, std::bitset<2>> reachability =
        std::make_tuple(imp_exp, exp_imp, exp_exp);

      /*
        Perform matching and updating of matrices
        .........................................
       */


      // Search for j_strs in T_1[i] and update matrices
      // Text => T_1[i]
      // Query => T_2[j]
      // -------------------------------------------------


      //std::cout << "Match positions:  \n";
      //std::cout << "text T1: " << i << " query T2: " << j << "\n";

      core::perform_matching(eds_t1,
                             i,
                             j,
                             w_suffix_trees[i],
                             eds_t2.get_strs(j),
                             &candidate_matches);

      // print candidate matches
      // -----------------------

      /*
      for (auto &m : candidate_matches) {
      std::cout << "-> txt str idx: " << m.get_txt_str_idx()  << " txt char idx: " << m.get_char_idx() << " len: " << m.get_match_length() << "\n";
      }
      */

      update_matrices(candidate_matches,
                      eds_t1,
                      eds_t2.get_slice(j),
                      &t1_matrix, &t2_matrix,
                      j, i,
                      reachability);

      candidate_matches.clear();

      // Search for i_strs in T_2[j] and update matrices
      // Query => T_1[i]
      // Text => T_2[j]
      // -----------------------------------------------

      //std::cout << "\n";
      //std::cout << "Match positions: \n";
      //std::cout << "text T2: " << j << " query T1: " << i << "\n";

      core::perform_matching(eds_t2,
                             j,
                             i,
                             q_suffix_trees[j],
                             eds_t1.get_strs(i),
                             &candidate_matches);

      /*
      for (auto &m : candidate_matches) {
      std::cout << "-> txt str idx: " << m.get_txt_str_idx()  << " txt char idx: " << m.get_char_idx() << " len: " << m.get_match_length() << "\n";
      }
      */


      update_matrices(candidate_matches,
                      eds_t2,
                      eds_t1.get_slice(i),
                      &t2_matrix, &t1_matrix,
                      i, j,
                      reachability);

      candidate_matches.clear();
    }
  }

  std::vector<std::size_t> q_cols = eds_t2.get_letter_ends_global(eds_t2.get_length() - 1);
  std::vector<std::size_t> w_cols = eds_t1.get_letter_ends_global(eds_t1.get_length() - 1);

  bool accept_w{false}, accept_q{false};
  for (size_t col : q_cols) {
    if (t2_matrix[len_t1 - 1][col]) { accept_q = true; break; }
  }

  for (size_t col : w_cols) {
    if (t1_matrix[len_t2 - 1][col]) { accept_w = true; break; }
  }

#ifdef DEBUG
  std::cout << std::endl;

  // print the T1 matrix
  for (std::size_t n{}; n < len_t2; n++) {
    for (std::size_t j{}; j < (size_t1 + eds_t1.get_eps_count()); j++) {
      std::cout << t1_matrix[n][j] << " ";
    }
    std::cout << std::endl;
  }

  std::cout << "-------" << std::endl;

  // print the T2 matrix
  for (std::size_t n{}; n < len_t1; n++) {
    for (std::size_t j{}; j < (size_t2 + eds_t2.get_eps_count()); j++) {
      std::cout << t2_matrix[n][j] << " ";
    }
    std::cout << std::endl;
  }
#endif

  return accept_q && accept_w;
}

} // namespace intersect::improved
