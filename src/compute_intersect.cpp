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

void gen_suffix_tree(
    EDS const &eds,                                               //
    size_t len,                                                   //
    std::vector<std::pair<STvertex, std::string>> *w_suffix_trees //
) {
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

match_data is_match_in_txt(int match_position, int qlen, std::vector<slicex> const *text_offsets) {
  if (false) {
    std::cerr << utils::indent(3)
              << "DEBUG, [improved::intersect::is_match_in_txt]"
              << std::endl;
  }

  for (size_t i = 0; i < text_offsets->size(); i++) {
    slicex sl = text_offsets->at(i);

    // how well does this work for eps
    // TODO: qlen is not needed 
    if (match_position + qlen <= sl.start + sl.length) {
      return match_data{
          .text_str_idx = (int)i,
          .text_start_idx = (match_position - (int)sl.start),
          .query_str_idx = -1,
          .query_start_idx = -1,
          .length = qlen,

      };
    }
  }

  return INVALID_MATCH;
}

std::vector<match_data> match(std::vector<suffix> const *suffixes,
                              std::vector<std::string> const *queries,
                              std::pair<STvertex, std::string> *text,
                              std::vector<slicex> const *text_offsets) {
  if (false) {
    std::cerr << utils::indent(3)
              << "DEBUG, [improved::intersect::match]"
              << std::endl;
  }

  std::vector<int> match_positions;
  std::vector<match_data> matches_found;

  for (auto suff = suffixes->begin(); suff != suffixes->end(); suff++) {
    //std::string query_str = queries->at(suff->str_idx);
    std::string query_str = queries->at(suff->str_idx).substr(suff->start_idx);

    // std::cerr << "query " << query_str << " text " << text->second.c_str() << std::endl ;
    match_positions = FindEndIndexes(query_str.c_str(), &text->first, text->second.c_str());

    // std::cerr << match_positions.size() << std::endl;

    if (match_positions.empty()) { continue; }

    for (int match_pos : match_positions) {

      // std::cerr << match_pos;

      match_data m = is_match_in_txt(match_pos, query_str.length(), text_offsets);

      // std::cerr << m << std::endl;

      if (m == INVALID_MATCH) { continue; }

      m.query_str_idx = suff->str_idx;
      m.query_start_idx = suff->start_idx;
      m.length = query_str.length();

      matches_found.push_back(m);
    }

    match_positions.clear();
  }

  return matches_found;

}

bool is_prev_letter_matched(int text_letter_idx,
                         int query_letter_idx,
                         match_data const match_found,
                         EDS const *query_eds,
                         EDS const *text_eds,
                         matrix *query_matrix,
                         matrix *text_matrix) {

  if (text_letter_idx == 0 && query_letter_idx == 0) { return true; }

  if (text_letter_idx == 1 && query_letter_idx == 0 && text_eds->data[0].has_epsilon) {
    return true;
  }

  if (query_letter_idx == 1 && text_letter_idx == 0 && query_eds->data[0].has_epsilon) {
    return true;
  }

  // loop over ...
  auto foo = [](int row_idx, std::vector<span> const *spans, matrix *m) {
    for (auto s = spans->begin(); s != spans->end(); s++) {
      if ((*m)[row_idx][s->stop]) { return true; }
    }
    return false;
  };

  if (text_letter_idx == 0) { // implied is query_letter_idx > 0
    bool has_eps = query_eds->data[query_letter_idx - 1].has_epsilon;
    bool eps_idx = query_eds->str_offsets[query_letter_idx - 1].back().stop;
    return has_eps && (*query_matrix)[0][eps_idx];

    // return foo(0, &query_eds->str_offsets[query_letter_idx - 1], query_matrix);
  }

  if (query_letter_idx == 0) { // implied is text_letter_idx > 0
    bool has_eps = text_eds->data[text_letter_idx-1].has_epsilon;
    bool eps_idx = text_eds->str_offsets[text_letter_idx - 1].back().stop;
    return has_eps && (*text_matrix)[0][eps_idx];
    // return foo(0, &text_eds->str_offsets[text_letter_idx - 1], text_matrix);
  }

  bool text_reachable, query_reachable = false;

  text_reachable = foo(query_letter_idx - 1, &text_eds->str_offsets[text_letter_idx - 1], text_matrix);
  query_reachable = foo(text_letter_idx - 1, &query_eds->str_offsets[query_letter_idx - 1], query_matrix);

  if (false) {
  std::cerr << "t idx " << text_letter_idx << " q idx " << query_letter_idx
            << " t: " << text_reachable << " q: " << query_reachable
            << std::endl;
  }
  return text_reachable && query_reachable;
}

bool is_prev_char_matched(int text_letter_idx, int query_letter_idx,
                          EDS const *text_eds, match_data const match_found,
                          matrix *text_matrix) {
  if (query_letter_idx > 0) {
    // TODO: do we want an extra check that the match is not in a str start?
    int col = text_eds->str_offsets[text_letter_idx][match_found.text_str_idx].start;
    col = (match_found.text_start_idx + col) - 1;

    return (*text_matrix)[query_letter_idx-1][col];
  }

  return false;
}

void update_matrices(int text_letter_idx,
                     int query_letter_idx,
                     std::vector<match_data> const *matches_found,
                     matrix *query_matrix,
                     matrix *text_matrix,
                     EDS const *query_eds,
                     EDS const *text_eds) {
  if (false) {
    std::cerr << utils::indent(3)
              << "DEBUG, [improved::intersect::update_matrices]"
              << std::endl;
  }

  int col;

  for (auto match_found = matches_found->begin(); match_found != matches_found->end(); match_found++) {
    if (false) {
    std::cerr << "txt ltr idx " << text_letter_idx << " qrt ltr idx " << query_letter_idx
              << std::endl << *match_found << std::endl;
    }

    // qry is exp text is exp
    if (match_found->is_exp_exp()) {
      if (is_prev_letter_matched(text_letter_idx, query_letter_idx, *match_found, query_eds, text_eds, query_matrix, text_matrix)) {
        // std::cerr << "matching" << std::endl;
        // update query matrix
        col = query_eds->str_offsets[query_letter_idx][match_found->query_str_idx].stop;
        (*query_matrix)[text_letter_idx][col] = true;
        // std::cerr << "query_matrix (" << text_letter_idx << ", " << col << ")" << std::endl;

            // update text matrix
            col =
            text_eds->str_offsets[text_letter_idx][match_found->text_str_idx]
                .start +
            match_found->length - 1;
        (*text_matrix)[query_letter_idx][col] = true;

        if (false) {
          std::cerr << "text_matrix (" << query_letter_idx << ", " << col << ")"
                    << std::endl;
        }
      }
    }

    // qry is imp text is exp
    if (match_found->is_imp_exp()) {
      // TODO: comfirm that exp is extendable
      // update query matrix
      col = query_eds->str_offsets[query_letter_idx][match_found->query_str_idx].stop;
      (*query_matrix)[text_letter_idx][col] = true;

      // update text matrix
      col = text_eds->str_offsets[text_letter_idx][match_found->text_str_idx].start + match_found->length - 1;
      (*text_matrix)[query_letter_idx][col] = true;
    }

    // qry is exp text is imp
    if (match_found->is_exp_imp()) {
      if (is_prev_char_matched(text_letter_idx, query_letter_idx, text_eds, *match_found, text_matrix)) {
        // update query matrix
        col = query_eds->str_offsets[query_letter_idx][match_found->query_str_idx].stop;
        (*query_matrix)[text_letter_idx][col] = true;

        // update text matrix
        col = text_eds->str_offsets[text_letter_idx][match_found->text_str_idx].start + match_found->text_start_idx + match_found->length - 1;
        (*text_matrix)[query_letter_idx][col] = true;
      }
    }
  }
}

/*

Is there an intersection between ED strings W and Q?

               Q
      0   ...    j   ...   n
      |--------------------|
    . |                    |
    . |                    |
    . |                    |
  W . |                    |
    i |                    |
    . |                    |
    . |                    |
    . |                    |
      |--------------------|
      m
*/

bool intersect(EDS &eds_w, EDS &eds_q, core::Parameters parameters) {
  if (parameters.verbosity > 1) { std::cerr << "DEBUG, [improved::intersect]" << std::endl; }

  size_t size_w = eds_w.size;
  size_t size_q = eds_q.size;

  int len_w = eds_w.length;
  int len_q = eds_q.length;

  matrix w_matrix = utils::gen_matrix(len_q, size_w + eds_w.epsilons);
  matrix q_matrix = utils::gen_matrix(len_w, size_q + eds_q.epsilons);

  //std::cerr << "N1: " << size_w << " N2: " << size_q << std::endl;

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

  if (false) {
    std::cerr << "INFO, [improved::intersect] Generating suffix trees"
              << std::endl;
  }

  gen_suffix_tree(eds_w, len_w, &w_suffix_trees);
  gen_suffix_tree(eds_q, len_q, &q_suffix_trees);

  // std::thread t1(gen_suffix_tree, std::ref(eds_w), len_w, &w_suffix_trees);
  // std::thread t2(gen_suffix_tree, std::ref(eds_q), len_q, &q_suffix_trees);
  // t1.join();
  // t2.join();

  timeRefRead = Time::now() - t0;

  if (false) {
    std::cerr
      << "INFO, [improved::intersect] Time spent generating suffix trees: "
      << timeRefRead.count() << " sec" << std::endl;
  }

  /*
    Find the intersection
    ---------------------
  */

  std::vector<int> match_positions;
  // TODO: move this upwards
  std::vector<match_data> matches_found; // how many could these be?
  std::vector<suffix> w_suffixes, q_suffixes;
  std::vector<span> const *letter_spans;

  auto get_str_idx = [](int col, std::vector<span> const *spans) -> int {
    for (int str_idx = 0; str_idx < spans->size(); str_idx++) {
      if (col < spans->at(str_idx).stop) {
        return str_idx;
      }
    }
    return -1; // error
  };

  auto get_active_suffixes = [&letter_spans, &get_str_idx](EDS const *query_eds,
                                                           int query_letter_idx,
                                                           int text_letter_idx,
                                                           matrix const *query_matrix,
                                                           std::vector<suffix> *active_suffixes) {
    letter_spans = &query_eds->str_offsets[query_letter_idx];
    for (int col = letter_spans->front().start; col <= letter_spans->back().stop; col++) {
      if (query_eds->stops.count(col) == 0 && (*query_matrix)[text_letter_idx - 1][col]) {
        // TODO: remove needless assignments
        int str_idx = get_str_idx(col, letter_spans);
        int start_idx = (col - letter_spans->at(str_idx).start) + 1;
        suffix suff =
            suffix{
                .query_letter_idx = query_letter_idx,
                .str_idx = str_idx,
                .start_idx = start_idx,
        };

        // std::cerr << suff << std::endl;

        active_suffixes->push_back(suff);
      }
    }
  };

  auto get_full_strings = [](int query_letter_idx,
                             int str_count,
                             std::vector<suffix> *active_suffixes) {
    for (int str_idx = 0; str_idx < str_count; str_idx++) {
      active_suffixes->push_back(suffix{
          .query_letter_idx = query_letter_idx,
          .str_idx = str_idx,
          .start_idx = 0,
      });
    }
  };

  for (int i = 0; i < len_w; i++) {
    for (int j = 0; j < len_q; j++) {

      if (false) {
        std::cerr << "-------------------------------"
                  << std::endl
                  << "(" << i << ", " << j << ")"
                  << std::endl;
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
          for (int col = eds_q.str_offsets[j].front().start; col <= eds_q.str_offsets[j].back().stop; col++) {
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
          for (int col = eds_w.str_offsets[i].front().start; col <= eds_w.str_offsets[i].back().stop; col++) {
            if (w_matrix[j - 1][col]) { w_matrix[j][col] = true; }
          }
        }
      }

      // Handle active prefixes / all queries
      // get W active suffixes
      if (j > 0) { get_active_suffixes(&eds_w, i, j, &w_matrix, &w_suffixes); }

      // get Q active suffixes
      if (i > 0) { get_active_suffixes(&eds_q, j, i, &q_matrix, &q_suffixes); }

      // search for j_strs in i
      get_full_strings(j, eds_q.data[j].data.size(), &q_suffixes);
      matches_found = match(&q_suffixes, &eds_q.data[j].data, &w_suffix_trees[i], &eds_w.str_slices[i]);
      update_matrices(i, j, &matches_found, &q_matrix, &w_matrix, &eds_q, &eds_w);

      // search for i_strs in j
      get_full_strings(i, eds_w.data[i].data.size(), &w_suffixes);
      matches_found = match(&w_suffixes, &eds_w.data[i].data, &q_suffix_trees[j], &eds_q.str_slices[j]);
      update_matrices(j, i, &matches_found, &w_matrix, &q_matrix, &eds_w, &eds_q);

      q_suffixes.clear();
      w_suffixes.clear();
    }
  }

  size_t_vec q_cols = utils::compute_accepting_states(eds_q);
  size_t_vec w_cols = utils::compute_accepting_states(eds_w);

  bool accept_w, accept_q = false;
  for (size_t col: q_cols) {
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

  auto prev_matched =
    [&dp_matrix, &linear_w, &linear_q, &eds_w, &eds_q](int row, int col) -> bool {
    // std::cout << row << " " << col << "\n";
    std::vector<int> prev_w = linear_w.prev_chars[row];
    std::vector<int> prev_q = linear_q.prev_chars[col];

    // only happens at letter 0
    // TODO: what if only one is empty?
    if (prev_w.empty() || prev_q.empty()) {
      // in the same letter

      if (eds_q.str_offsets[0].back().stop >= col && eds_w.str_offsets[0].back().stop >= row) {
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
          for (int c = 0; c < last_col; c++) {
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
