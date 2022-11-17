
#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <utility>

#include "./utils.cpp"
#include "./gen_suffix_tree.cpp"




void test_lacks_intersect();
void test_handle_epsilon();
void test_contains_intersect();
void test_parse_ed_string();


// is at least one the strings in the letter_idx index of the eds given matched?
// it is matched is at least one of its indexes is true in the dp matrix
bool is_letter_matched(EDS &eds, // the eds we are checking
                       int letter_idx, // has any string in this d letter been matched
                       matrix &dp_matrix, // matrix that tracks matches in the eds
                       int row_idx // row in dp_matrix
                       ) {
  std::vector<span> &letter_span = eds.str_offsets[letter_idx];

  for (auto sp : letter_span) {
    if (dp_matrix[row_idx][sp.stop]) {
      return true;
    }
  }

  return false;
};

// is at least one the strings in the letter_idx index of the eds given matched?
// it is matched is at least one of its indexes is true in the dp matrix
bool is_prev_letter_matched(
    EDS &eds,          // the eds we are checking
    int letter_idx,    // has any string in this d letter been matched
    matrix &dp_matrix, // matrix that tracks matches in the eds
    int row_idx        // row in dp_matrix
){

  if (row_idx == 0) {
    return true;
  }

  return is_letter_matched(eds, letter_idx, dp_matrix, row_idx-1);
};

bool is_prefix_matched(EDS &eds, //
                       matrix &dp_matrix, //
                       int pos, // the position in which our match started
                       size_t row, // the letter whose chars we are matching in
                       size_t comp  // the letter whose strings we are matching against
                       ) {
  // TODO: should this be a one liner?
  // can we extend from within this current degenerate letter?
  size_t letter_start_in_size = eds.str_offsets[comp].front().start;
  size_t match_start_in_size = pos + letter_start_in_size;
  return dp_matrix[row][match_start_in_size - 1];
}

// filter results from ST
bool is_match_valid(vector<span> &spans,
                    int letter_idx,
                    std::size_t query_length,
                    int match_position,
                    int letter_start_in_n
                    ) {
  for (auto sp : spans) {
    int k = match_position;
    if (sp.stop >= k && (k + query_length - 1) > sp.stop) {
      return false;
    }
  }

  return true;
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

    std::vector<int> q_end_indexes, w_end_indexes; // TODO: allocate one large one
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

      if (DEBUG_LEVEL > 3) { printf("\tj text: %s\n", root_text.second.c_str()); }

      // handle epsilon
      if (eds_w.data[i].has_epsilon &&
          (j == 0 ||
           is_letter_matched(eds_w, i - 1, w_matrix, j))
          ) {
        std::vector<span> letter_span = eds_w.str_offsets[i];
        int epsilon_idx = letter_span.back().stop;

        w_matrix[j][epsilon_idx] = true;
        q_end_indexes.push_back(epsilon_idx);
        inc_i = true;
      }

      for (auto i_str : eds_w.data[i].data) {
        // if (DEBUG_LEVEL > 3) { printf("\t\ti_str: %s\n", i_str.c_str()); }

        vector<int> res = FindEndIndexes(i_str.c_str(), &root_text.first, root_text.second.c_str());

        // if the string does not match anywhere
        if (res.empty()) { continue; }

        std::vector<span> spans = eds_q.str_offsets[j];
        int letter_start_in_n = spans.front().start;

        for (int pos: res) {

          if (DEBUG_LEVEL > 3) { printf("\t\ti_str: %s pos: %d\n", i_str.c_str(), pos); }

          // if (!is_match_valid(spans, j, i_str.length(), pos, letter_start_in_n)) { continue; }

          if (is_match_valid(spans, j, i_str.length(), pos, letter_start_in_n) &&
              (i == 0 ||
               is_letter_matched(eds_q, j - 1, q_matrix, i) ||
               is_prefix_matched(eds_q, q_matrix, pos, i, j))
              ) {
            int end_idx = letter_start_in_n + pos + i_str.length() - 1;
            q_matrix[i][end_idx] = true;
            q_end_indexes.push_back(end_idx);
            inc_i = true;

            if (DEBUG_LEVEL > 4) { printf("\t\tpos %d  \tpos_in_n: %d\n", pos, end_idx); }
          }
        }
      }

      // -------
      // match in Q
      // -------

      w_end_indexes.clear();
      root_text = w_suffix_trees[i];
      if (DEBUG_LEVEL > 3) { printf("\ti text: %s\n", root_text.second.c_str()); }

      // handle epsilon
      if (eds_q.data[j].has_epsilon &&
          (i == 0 ||
           is_letter_matched(eds_q, j - 1, q_matrix, i))
          ) {
        std::vector<span> letter_span = eds_q.str_offsets[j];
        int epsilon_idx = letter_span.back().stop;

        q_matrix[i][epsilon_idx] = true;
        w_end_indexes.push_back(epsilon_idx);
        inc_j = true;
      }

      std::vector<std::string> j_strings =  eds_q.data[j].data;

      for (auto j_str : j_strings) {

        vector<int> res = FindEndIndexes(j_str.c_str(), &root_text.first, root_text.second.c_str());

        // if the string does not match anywhere
        if (res.empty()) { continue; }

        if (DEBUG_LEVEL > 3) { printf("\t\tj_str: %s\n", j_str.c_str()); }

        std::vector<span> spans = eds_w.str_offsets[i];
        int letter_start_in_n = spans.front().start;

        for (int pos : res) {

          if (DEBUG_LEVEL > 3) { printf("\t\t\tpos: %d\n", pos); }

          // if (!is_match_valid(spans, i, j_str.length(), pos, letter_start_in_n)) { continue; }

          if (is_match_valid(spans, i, j_str.length(), pos, letter_start_in_n) &&
              (j == 0 ||
              is_letter_matched(eds_w, i - 1, w_matrix, j) || // TODO: what if i == 0?
               is_prefix_matched(eds_w, w_matrix, pos, j, i))
              ) {
            int end_idx = letter_start_in_n +  pos + j_str.length() - 1;
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
      auto inc_and_copy = [](matrix &dp_matrix, std::size_t* row, int max_row, std::vector<int> &cols){
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

int main() {
  // test_handle_epsilon();
  test_contains_intersect();
  test_lacks_intersect();
  // test_parse_ed_string();
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

  std::vector<std::vector<span>> str_offsets = eds.str_offsets;

  // TODO: confirm size, len etc
}


void test_contains_intersect() {
  std::string ed_string_w = "{AT,TC}{ATC,T}";
  std::string ed_string_q = "{TC,G}{CT,T}";

  EDS eds_w = parse_ed_string(ed_string_w);
  EDS eds_q = parse_ed_string(ed_string_q);

  IS_TRUE(intersect(eds_w, eds_q));
}

void test_handle_epsilon() {
  std::string ed_string_w, ed_string_q;
  EDS eds_w, eds_q;

  ed_string_w = "{AT,TC}{ATC,T}";
  ed_string_q = "TC{,G}{CT,T}";
  eds_w = parse_ed_string(ed_string_w);
  eds_q = parse_ed_string(ed_string_q);
  IS_TRUE(intersect(eds_w, eds_q));

  ed_string_w = "{AT,TC}{ATC,T}";
  ed_string_q = "{,G}{CT,T}";
  eds_w = parse_ed_string(ed_string_w);
  eds_q = parse_ed_string(ed_string_q);
  // IS_TRUE(intersect(eds_w, eds_q));

  ed_string_w = "{AT,TC}{ATC,}";
  ed_string_q = "{TC,G}{CT,T}";
  eds_w = parse_ed_string(ed_string_w);
  eds_q = parse_ed_string(ed_string_q);
  // IS_TRUE(intersect(eds_w, eds_q));

  ed_string_w = "{AT,TC}{ATC,A}";
  ed_string_q = "{,G}{CT,T}";
  eds_w = parse_ed_string(ed_string_w);
  eds_q = parse_ed_string(ed_string_q);
  // IS_TRUE(!intersect(eds_w, eds_q));
}

void test_lacks_intersect() {
  std::string ed_string_w = "{AT,TC}{ATC,T}";
  std::string ed_string_q = "{TC,G}{CG,G}";

  EDS eds_w = parse_ed_string(ed_string_w);
  EDS eds_q = parse_ed_string(ed_string_q);

  IS_TRUE(!intersect(eds_w, eds_q));
}
