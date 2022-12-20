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
#include <utility>
#include <vector>

#include "./parser.cpp"
#include "./suffix_tree.cpp"
#include "core.hpp"


namespace improved {


/*
  Check that the match is valid i.e within a text string
  return which string the match is in and where in the string the
  match starts
*/
match_info compute_match_info(vector<spread> const &txt_spans, //
                              int txt_letter_idx,              //
                              int qry_letter_idx,              //
                              int match_position,              //
                              int qlen,                        //
                              suffix suff,                     //
                              std::set<suffix> const &txt_active_suffixes,
                              matrix const &txt_matrix, //
                              matrix const &qry_matrix,  //
                              core::ed_string in,   //
                              std::set<cell> *txt_reachable_epsilons,
                              std::set<cell> *qry_reachable_epsilons
                              ) {
  if (false) {
    std::cerr << utils::indent(3)
              << "DEBUG, [improved::intersect::compute_match_info]"
              << std::endl;
  }

  if (false) {
    std::cerr << utils::indent(4)
              << "suffix " << suff
              << " match_pos: " << match_position
              << std::endl;
  }

  int match_str_idx = -1;
  int str_count = (int)txt_spans.size();
  match_info invalid_match = match_info { .str_idx = match_str_idx, .start_idx = -1 };

  // is the active suffix extendable
  bool is_qry_active_suffix = suff.query_letter_idx != -1;

  // was this suffix created by a partial match in the previous letter?
  bool from_prev_letter = false;
  if (txt_letter_idx > 0) {
    from_prev_letter = suff.query_letter_idx == (txt_letter_idx - 1);
  }

  if (is_qry_active_suffix && !from_prev_letter) { return invalid_match; }

  if (false) {
    std::cerr << utils::indent(4)
              << "is active suffix " << is_qry_active_suffix
              << ", frm prev ltr " << from_prev_letter
              << std::endl;
  }

  // Find the string in which the match occurred
  // Set match_str_idx to -1, if we go out of the string in which the match occurred
  spread s;
  int end, match_end;
  for (int span_idx = 0; span_idx < str_count; span_idx++) {
    s = txt_spans[span_idx];
    end  = s.start + s.len;
    match_end = match_position + qlen;

    if (match_position == s.start) {
      if ((is_qry_active_suffix && qlen <= s.len) || qlen <= s.len) {
        match_str_idx = span_idx;
        break;
      } else {
        match_str_idx = -1;
      }
    }

    if (match_position < end && match_end < end) {
      match_str_idx = span_idx;
      break;
    }
  }

  if (match_str_idx < 0 ) { return invalid_match; }

  // Find the start index of the match (start_idx) within
  // the matched string (match_str_idx)
  int prev_lens = 0;
  for (int span_idx = 0; span_idx < match_str_idx; span_idx++) {
    prev_lens += txt_spans[span_idx].len;
  }
  int match_start_idx = match_position - prev_lens;

  // validate the match
  // ------------------

  // If the match occured internally, can we extend an active suffix?
  if (match_start_idx > 0) {
    // to confirm validity, create the suffix that would extend and
    // see if it exists in the set
    if ( qry_letter_idx > 0 ) {
      suffix s = {.query_letter_idx = qry_letter_idx -1, .str_idx = match_str_idx, .start_idx = match_start_idx };
      if ( !txt_active_suffixes.count(s) ) { return invalid_match; }
    } else {
      return invalid_match;
    }
  }

  // && !(txt_letter_idx == 0 && qry_letter_idx == 0)


  // entire strings against each other
  if ( match_start_idx == 0 && !is_qry_active_suffix ) {
    bool prev_txt_reachable, prev_qry_reachable;
    int prev_txt_ltr = txt_letter_idx == 0 ? txt_letter_idx : txt_letter_idx - 1;
    int prev_qry_ltr = qry_letter_idx == 0 ? qry_letter_idx : qry_letter_idx - 1;

    bool is_txt_zero = prev_txt_ltr == txt_letter_idx;
    bool is_qry_zero = prev_qry_ltr == qry_letter_idx;

    if (is_txt_zero && is_qry_zero) {
      prev_qry_reachable = true;
      prev_txt_reachable = true;
    }

    if (!is_txt_zero && !is_qry_zero) {
      if (in == core::ed_string::q) {
        bool both_eps =
          qry_reachable_epsilons->count(cell{.j = prev_txt_ltr, .i =  prev_qry_ltr}) &&
          txt_reachable_epsilons->count(cell{.j = prev_txt_ltr, .i = prev_qry_ltr});

        prev_qry_reachable = qry_matrix[prev_txt_ltr][prev_qry_ltr];
        // qry_reachable_epsilons->count(cell{.j = prev_txt_ltr, .i =
        // prev_qry_ltr});
        prev_txt_reachable = txt_matrix[prev_txt_ltr][prev_qry_ltr]; // ||
        // txt_reachable_epsilons->count(cell{.j = prev_txt_ltr, .i =
        // prev_qry_ltr});

        if (both_eps) { prev_txt_reachable = prev_qry_reachable = true; }

      } else {
        bool both_eps =
          qry_reachable_epsilons->count(cell{.j = prev_qry_ltr, .i = prev_txt_ltr}) &&
          txt_reachable_epsilons->count(cell{.j = prev_qry_ltr, .i = prev_txt_ltr});

        prev_qry_reachable = qry_matrix[prev_qry_ltr][prev_txt_ltr]; // ||
        // qry_reachable_epsilons->count(cell{.j = prev_qry_ltr, .i =
        // prev_txt_ltr});
        prev_txt_reachable = txt_matrix[prev_qry_ltr][prev_txt_ltr]; // ||
        // txt_reachable_epsilons->count(cell{.j = prev_qry_ltr, .i =
        // prev_txt_ltr});;

        if (both_eps) { prev_txt_reachable = prev_qry_reachable = true; }
      }
    }

    if (is_txt_zero && !is_qry_zero) {
      prev_txt_reachable = true;
      if (in == core::ed_string::q) {
        prev_qry_reachable = qry_reachable_epsilons->count(cell{.j = prev_txt_ltr, .i = prev_qry_ltr});
      } else {
        prev_qry_reachable  = qry_reachable_epsilons->count(cell{.j = prev_qry_ltr, .i = prev_txt_ltr});
      }
    }

    if (is_qry_zero && !is_txt_zero) {
      prev_qry_reachable = true;
      if (in == core::ed_string::q) {
        prev_txt_reachable =txt_reachable_epsilons->count(cell{.j = prev_txt_ltr, .i = prev_qry_ltr});
      } else {
        prev_txt_reachable = txt_reachable_epsilons->count(cell{.j = prev_qry_ltr, .i = prev_txt_ltr});;
      }
    }

    bool prev_letters_matched = prev_qry_reachable && prev_txt_reachable;
    if (!prev_letters_matched) { return invalid_match; }
  }

  return match_info{.str_idx = match_str_idx, .start_idx = match_start_idx};
}

void handle_epsilon(matrix *w_matrix,   //
                    matrix *q_matrix,   //
                    int fixed,          // other
                    int to,             // idx of letter with epsilon
                    core::ed_string in, // eds with epsilon
                    std::vector<std::set<suffix>> *active_suffixes, // active suffixes of the other
                    int txt_d_letter_str_count, //
                    std::set<cell> *w_reachable_epsilons,
                    std::set<cell> *q_reachable_epsilons
                    ) {
  if (false) {
    std::cerr << utils::indent(2)
              << "INFO, [improved::intersect::handle_epsilon]"
              << std::endl;
  }

  if (false) {
    std::cerr << utils::indent(2)
              << "in: " << (in == core::ed_string::q ? "Q" : "W")
              << std::endl;
  }

  // make all strings active suffixes
  for (int idx = 0; idx < txt_d_letter_str_count; idx++) {
    suffix s = suffix{.query_letter_idx = to, .str_idx = idx, .start_idx = 0};
    (*active_suffixes)[fixed].insert(s);
  }

  if (to == 0) {
    // by default reachable
    if (in == core::ed_string::q) {
      q_reachable_epsilons->insert(cell{.j = to, .i = fixed});
      //(*q_matrix)[to][fixed] = true;

      // (*w_matrix)[to][fixed] = true;
    } else {
      w_reachable_epsilons->insert(cell{.j = fixed, .i = to});
      // (*w_matrix)[fixed][to] = true;

      // (*q_matrix)[to][fixed] = true;

    }
  } else {

    int from = to - 1;

    // if we can reach prev we can reach this one
    if (in == core::ed_string::q) {
      // in q we copy over 1s from row j-1 to j

      if ( (*q_matrix)[from][fixed] || q_reachable_epsilons->count(cell{.j = from, .i = fixed}) ) {
        q_reachable_epsilons->insert(cell{.j = to, .i = fixed});
      }
      if ( (*w_matrix)[from][fixed] || w_reachable_epsilons->count(cell{.j = from, .i = fixed})) {
        w_reachable_epsilons->insert(cell{.j = to, .i = fixed});
      }

      // if ( (*q_matrix)[from][fixed] ) { (*q_matrix)[to][fixed] = true; }
      // if ( (*w_matrix)[from][fixed] ) { (*w_matrix)[to][fixed] = true; }
    } else {
      // in w we copy over 1s from row i-1 to i
      bool prev_q_reachable =
        (*q_matrix)[fixed][from] ||
        q_reachable_epsilons->count(cell{.j = fixed, .i = from});
      bool prev_w_reachable =
        (*w_matrix)[fixed][from] ||
        w_reachable_epsilons->count(cell{.j = fixed, .i = from});

      if (false) {
        std::cerr << utils::indent(3)
                  << "prev reachable w: " << prev_w_reachable
                  << " q: " << prev_q_reachable
                  << std::endl;
      }

      if ((*q_matrix)[fixed][from] || q_reachable_epsilons->count(cell{.j = fixed, .i = from})) {
        q_reachable_epsilons->insert(cell{.j = fixed, .i = to});
      }
      if ( (*w_matrix)[fixed][from] || w_reachable_epsilons->count(cell{.j = fixed, .i = from})) {
        w_reachable_epsilons->insert(cell{.j = fixed, .i = to});
      }

      // if ((*q_matrix)[fixed][from]) { (*q_matrix)[fixed][to] = true; }
      // if ((*w_matrix)[fixed][from]) { (*w_matrix)[fixed][to] = true; }
    }
    // copy active suffixes created by from to also include to
    for (int fxd = 0; fxd <= fixed; fxd++) {
      std::set<suffix> *suffixes = &(*active_suffixes)[fxd];
      for (suffix suff : *suffixes) {

        if (false) {
          std::cerr << utils::indent(3)
                    << "fixed: " << fixed
                    << " from: " << from
                    << " to: " << to
                    << " suffix: " << suff
                    << " to save? " << (suff.query_letter_idx == from)
                    << std::endl;
        }

        if (suff.query_letter_idx == from) {
          suffixes->insert(suffix{.query_letter_idx = to, .str_idx = suff.str_idx, .start_idx = suff.start_idx});
        }
      }
    }
  }
}

// TODO: do this while reading the EDT
void compute_letter_spans(string_vec d_letter_data, //
                          std::vector<std::vector<spread>>* eds_spans //
                          ) {
  std::vector<spread> spans;
  spans.reserve(d_letter_data.size());

  int str_start = 0;

  for (std::string str : d_letter_data) {
    spans.push_back(spread{.start = str_start, .len = (int)str.length()});
    str_start += str.length();
  }

  eds_spans->push_back(spans);
}

std::vector<std::vector<spread>> compute_spans(EDS &eds) {
  std::vector<std::vector<spread>> eds_spans;
  eds_spans.reserve(eds.length);

  for (degenerate_letter d_letter : eds.data) {
    compute_letter_spans(d_letter.data, &eds_spans);
  }

  return eds_spans;
}

std::set<suffix> extract_suffixes(string_vec const& qry_d_letter_data,
                                  int txt_letter_idx,
                                  std::set<suffix> const& qry_suffixes
                                  ) {
    if (false) {
      std::cerr << utils::indent(3)
                << "DEBUG, [improved::intersect::extract_suffixes]"
                << std::endl;
    }

    std::set<suffix> s;

    // full strings
    for(int idx=0; idx < qry_d_letter_data.size(); idx++) {
      s.insert(suffix{.query_letter_idx = -1, .str_idx = idx, .start_idx = 0});
    }

    if (txt_letter_idx == 0) { return s; }

    // we may have an active suffix from a predecessor
    // active suffixes
    for (suffix suff : qry_suffixes) {

      if (false) { std::cerr << utils::indent(4) << suff << std::endl; }

      bool from_epsilon = suff.start_idx == 0;

      if (suff.query_letter_idx == txt_letter_idx - 1 && !from_epsilon) { s.insert(suff); }
    }

    return s;
}

void match(matrix *txt_matrix, //
           matrix *qry_matrix, //
           EDS &query_eds, //
           std::pair<STvertex, std::string> suffix_tree, //
           std::vector<spread> &txt_spans, //
           int txt_letter_idx, //
           core::ed_string in, //
           int qry_letter_idx, //
           std::set<suffix> *txt_active_suffixes, //
           std::set<suffix> *qry_active_suffixes, //
           std::set<cell> *txt_reachable_epsilons,
           std::set<cell> *qry_reachable_epsilons
           ) {
  if (false) {
    std::cerr << utils::indent(2)
              << "txt: " << (in == core::ed_string::q ? "Q" : "W")
              << std::endl;
  }
  string_vec const& qry_d_letter_data = query_eds.data[qry_letter_idx].data;

  std::set<suffix> suffixes = extract_suffixes(qry_d_letter_data,
                                               txt_letter_idx,
                                               *qry_active_suffixes);

  if (false) {
    std::cerr << utils::indent(2)
              << "num suffixes " << suffixes.size()
              << std::endl;
  }

  for (suffix suff : suffixes) {
    if (false) {
      std::cerr << utils::indent(3)
                << "{ qry ltr: " << suff.query_letter_idx
                << " str idx: " << suff.str_idx
                << " start idx: " << suff.start_idx
                << " }" << std::endl;
    }
  }

  for (suffix suff : suffixes) {
    if (false) {
      std::cerr << utils::indent(2)
                << "suff: str idx: " << suff.str_idx
                << " start idx: " << suff.start_idx
                << std::endl;
    }
    std::string qry_str = qry_d_letter_data[suff.str_idx].substr(suff.start_idx);


    vector<int> match_positions = FindEndIndexes(qry_str.c_str(), &suffix_tree.first,
                                     suffix_tree.second.c_str());

    if (false) {
      std::cerr << utils::indent(2)
                << "query str: " << qry_str
                << " txt str: " << suffix_tree.second.c_str()
                << " num matches: " << match_positions.size()
                << std::endl;
    }

    // if the string does not match anywhere
    if (match_positions.empty()) { continue; }

    int qlen = qry_str.length();

    for (int match_position : match_positions) {
      if (false) {
        std::cerr << utils::indent(2)
                  << "Checking match pos: " << match_position
                  << std::endl;
      }

      match_info m_info = compute_match_info(txt_spans, txt_letter_idx,
                                             qry_letter_idx,
                                             match_position, qlen, suff,
                                             *txt_active_suffixes,
                                             *txt_matrix, *qry_matrix, in,
                                             txt_reachable_epsilons,
                                             qry_reachable_epsilons
                                             );

      if (false) {
        std::cerr << utils::indent(2)
                  << "match info {"
                  << " match str idx: " << m_info.str_idx
                  << ", match start idx: " << m_info.start_idx
                  << " }"
                  << std::endl;
      }

      if (m_info.start_idx < 0) { continue; }
      int tlen = txt_spans[m_info.str_idx].len;

      // if greater then we have a problem in compute_match_info
      if ((m_info.start_idx + qlen) > tlen) {
        std::cerr << "INFO, [improved::match] Error in match" << std::endl;
        exit(1);
      }

      bool reaches_end = (m_info.start_idx + qlen) == tlen;

      bool matches_entirely = m_info.start_idx == 0 && tlen == qlen;
      bool qry_is_prefix = m_info.start_idx == 0 && !reaches_end;
      bool qry_is_substr = m_info.start_idx > 0 && !reaches_end;
      bool qry_is_suffix = m_info.start_idx > 0 && reaches_end;

      if (false) {
        std::cerr << utils::indent(2)
                  << "Found: "
                  << std::endl
                  << utils::indent(3)
                  << "qry str idx: " << suff.str_idx
                  << " matches entirely: " << matches_entirely
                  << " qry is substr: " << qry_is_substr
                  << " suffix: " << qry_is_suffix
                  << " prefix: " << qry_is_prefix
                  << std::endl;
      }

      if (matches_entirely || qry_is_suffix) {
        if (in == core::ed_string::q) {
          (*qry_matrix)[txt_letter_idx][qry_letter_idx] = true;
          (*txt_matrix)[txt_letter_idx][qry_letter_idx] = true;
        } else {
          (*qry_matrix)[qry_letter_idx][txt_letter_idx] = true;
          (*txt_matrix)[qry_letter_idx][txt_letter_idx] = true;
        }
        continue;
      }

      // qry must be a prefix of the text or a substring of the text
      // -----------------

      // save matches
      if (in == core::ed_string::q) {
        // save W match
        (*qry_matrix)[txt_letter_idx][qry_letter_idx] = true;
      } else {
        // save Q match
        (*qry_matrix)[qry_letter_idx][txt_letter_idx] = true;
      }

      // create active suffix
      suffix s = suffix{.query_letter_idx = qry_letter_idx,
                        .str_idx = m_info.str_idx,
                        .start_idx = m_info.start_idx + qlen
      };

      txt_active_suffixes->insert(s);

      if (false) {
        std::cerr << utils::indent(2)
                  << "Created active suffix: " << s << std::endl;
      }
    }
  }
}

void gen_suffix_tree(EDS const& eds, //
                     size_t len, //
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
  if (parameters.verbosity > 1) { std::cerr << "DEBUG, [improved::intersect]\n" << std::endl; }

  // size_t size_w = eds_w.size;
  // size_t size_q = eds_q.size;

  int len_w = eds_w.length;
  int len_q = eds_q.length;

  matrix w_matrix = utils::gen_matrix(len_q, len_w);
  matrix q_matrix = utils::gen_matrix(len_q, len_w);

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

  std::vector<std::set<suffix>> w_active_suffixes(len_w, std::set<suffix>());
  std::vector<std::set<suffix>> q_active_suffixes(len_q, std::set<suffix>());

  std::vector<std::vector<spread>> w_spans = compute_spans(eds_w);
  std::vector<std::vector<spread>> q_spans = compute_spans(eds_q);

  bool diagonal, above, left;
  std::set<cell> q_reachable_epsilons, w_reachable_epsilons;

  std::cerr << std::endl << std::endl;

  for (int i = 0; i < len_w; i++) {
    for (int j = 0; j < len_q; j++) {

      if (false) {
        std::cerr << std::endl << utils::indent(1)
                  << "letter i: " << i << " j: " << j
                  << std::endl
                  << utils::indent(1) << "---------------------"
                  << std::endl;
      }

      // Handle epsilon
      // --------------
      if (eds_w.data[i].has_epsilon) {
        handle_epsilon(&w_matrix, &q_matrix, j, i, core::ed_string::w,
                       &q_active_suffixes, eds_q.data[j].data.size(),
                       &w_reachable_epsilons, &q_reachable_epsilons
                       );
      }
      if (eds_q.data[j].has_epsilon) {
        handle_epsilon(&w_matrix, &q_matrix, i, j, core::ed_string::q,
                       &w_active_suffixes, eds_w.data[i].data.size(),
                       &w_reachable_epsilons, &q_reachable_epsilons
                       );
      }

      // Confirm reachability
      // --------------------

      auto reached = [&](int row, int col) -> bool {

        bool i_reachable = (w_matrix[row][col] || w_reachable_epsilons.count(cell{.j = row, .i = col}));
        bool j_reachable = (q_matrix[row][col] || q_reachable_epsilons.count(cell{.j = row, .i = col}));

        if (false) {
          std::cerr << utils::indent(2)
                    << "row: " << row
                    << " col: " << col
                    << " reachable i: " << i_reachable
                    << " j: " << j_reachable
                    << std::endl;
        }

        if (i_reachable && j_reachable) {
          return true;
        }

        if (i_reachable && !j_reachable) {
          for (auto suff : q_active_suffixes[row]) {
            if (suff.query_letter_idx == col) { return true; }
          }
        }

        if (!i_reachable && j_reachable) {
          for (auto suff : w_active_suffixes[col]) {
            if (suff.query_letter_idx == row) { return true; }
          }
        }

        return false;
      };

      // check if [j][i] is reachable, if not skip this loop iteration
      if (j>0 && i>0) {
        diagonal = reached(j-1, i-1);
      } else {
        diagonal = false;
      }

      if (j>0) {
        above = reached(j-1, i);
      } else {
        above = false;
      }

      if (i > 0) {
        left = reached(j, i-1);
      } else {
        left = false;
      }

      if (j == 0 && i == 0) { left = above = diagonal = true; }

      if (false) {
        std::cerr << utils::indent(1)
                  << "diagonal: " << diagonal
                  << " above: " << above
                  << " left: " << left
                  << std::endl;
      }

      if (!diagonal && !above && !left) { continue; }

      // Perform matching
      // ----------------

      // match W
      match(&w_matrix, &q_matrix, eds_q, w_suffix_trees[i],
            w_spans[i], i, core::ed_string::w, j,
            &w_active_suffixes[i], &q_active_suffixes[j],
            &w_reachable_epsilons, &q_reachable_epsilons);
      // match Q
      match(&q_matrix, &w_matrix, eds_w, q_suffix_trees[j],
            q_spans[j], j, core::ed_string::q, i,
            &q_active_suffixes[j], &w_active_suffixes[i],
            &q_reachable_epsilons, &w_reachable_epsilons);

      if (false) { std::cerr << std::endl << std::endl; }
      }
  }

  if (false) {
    printf(" Improved \n");
    std::cerr << std::endl << std::endl;
    printf("-------------W----------\n");
    utils::print_matrix(w_matrix);
    printf("-------------Q----------\n");
    utils::print_matrix(q_matrix);
    printf("\n\n");
  }

  if (false) {
    std::cout << "-------------W----------" << std::endl;
    utils::print_edt_range(eds_w, 0, 24);
    std::cout << std::endl << "-------------Q----------" << std::endl;
    utils::print_edt_range(eds_q, 0, 24);
    std::cout << std::endl;
  }

  bool eps_end = q_reachable_epsilons.count(cell{.j = len_q-1, .i = len_w-1}) &&
    w_reachable_epsilons.count(cell{.j = len_q-1, .i = len_w-1});
  bool str_end = w_matrix[len_q - 1][len_w - 1] && q_matrix[len_q - 1][len_w - 1];

  return str_end || eps_end;
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
  if (parameters.verbosity > 1) { printf("DEBUG, [naive::intersect]\n"); }

  LinearizedEDS linear_w = parser::linearize(eds_w);
  LinearizedEDS linear_q = parser::linearize(eds_q);

  size_t last_row = linear_w.str.length();
  size_t last_col = linear_q.str.length();

  boolean_matrix dp_matrix = utils::gen_matrix(last_row, last_col);
  // boolean_matrix q_dp_matrix = utils::gen_matrix(last_row, last_col);

  auto prev_matched = [&dp_matrix, &linear_w, &linear_q, &eds_w,
                       &eds_q](int row, int col) -> bool {
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
