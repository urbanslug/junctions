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
void test_contains_intersect();
void test_parse_ed_string();

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

    matrix w_matrix = gen_matrix(len_w, size_w);
    matrix q_matrix = gen_matrix(len_q, size_q);

    /*
      Preprocess suffix trees
      -----------------------
    */
    auto gen_suffix_tree =
        [](EDS &eds, size_t len,
           std::vector<std::pair<STvertex, std::string>> *w_suffix_trees) {
          size_t i = 0;
          while (i < len) {
            degenerate_letter i_letter = eds.data[i];
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
    auto can_extend =
        [](EDS &eds,
           matrix dp_matrix,
           int pos,
           std::string &j_str,
           size_t curr, // the letter whose chars we are matching in
           size_t comp  // the letter whose strings we are matching against
           ) -> bool {
      if (DEBUG_LEVEL > 4) {
        printf("\t\t[cpp::main::intersect::can_extend]\n");
      }

      // printf("\t\t\tpos: %d\n", pos);

      // TODO: maybe needlessly reptitive & assignments
      size_t letter_start_in_n = eds.str_offsets[comp][0].start;
      size_t start = pos - j_str.length() + 1;

      start += letter_start_in_n;

      // TODO: is it better to call  compute_str_starts ?
      // is start in the start of a degenerate letter?
      bool check_prev_letter = false;
      for (auto s : eds.str_offsets[comp]) {
        // printf("(%lu, %lu)", s.start, start);
        if (s.start == start) {
          check_prev_letter = true;
          break;
        }
      }

      // printf("\t\t\tletter start: %lu pos %d curr start %lu prev : %d\n",
      //       letter_start_in_n, pos, start, check_prev_letter);

      if (check_prev_letter) {
        // check whether any of the previous letter ends are true
        for (auto sp : eds.str_offsets[comp - 1]) {
          if (dp_matrix[curr - 1][sp.stop] || dp_matrix[curr][sp.stop]) {
            return true;
          }
        }
      } else {
        // we are extending within the same degenerate letter i
        if (dp_matrix[curr - 1][start - 1] || dp_matrix[curr][start - 1]) {
          return true;
        }
      }

      return false;
    };

    size_t i, j;
    i = j = 0;
    bool inc_i = false, inc_j = false;
    while (i < len_q && j < len_w) {
      if (DEBUG_LEVEL > 3) {
        printf("\tletter i: %lu, j: %lu\n", i, j);
        printf("\t----------------\n");
      }

      inc_i = inc_j = false;
      std::pair<STvertex, std::string> root_text;
      root_text = q_suffix_trees[j];

      if (DEBUG_LEVEL > 3) {
        printf("\tj text: %s\n", root_text.second.c_str());
      }

      for (auto i_str : eds_w.data[i]) {
        // if (DEBUG_LEVEL > 3) { printf("\t\ti_str: %s\n", i_str.c_str()); }

        vector<int> res = FindEndIndexes(i_str.c_str(), &root_text.first,
                                         root_text.second.c_str());

        if (res.empty()) {
          continue;
        } // if pos is not found

        for (int pos: res) {
          if (DEBUG_LEVEL > 3) {
            printf("\t\ti_str: %s pos: %d\n", i_str.c_str(), pos);
          }

          pos = pos + i_str.length() - 1;

          if (i == 0) {
            q_matrix[i][pos] = true;
            inc_i = true;
            continue;
          }

          if ((can_extend(eds_q, q_matrix, pos, i_str, i, j))) {
            size_t pos_in_n = eds_q.str_offsets[j][0].start + pos;
            q_matrix[i][pos_in_n] = true;
            inc_i = true;

            if (DEBUG_LEVEL > 4) {
              printf("\t\tpos %d  \tpos_in_n: %lu\n", pos, pos_in_n);
            }
          }
        }
      }

      root_text = w_suffix_trees[i];
      if (DEBUG_LEVEL > 3) {
        printf("\ti text: %s\n", root_text.second.c_str());
      }
      for (auto j_str : eds_q.data[j]) {

        vector<int> res = FindEndIndexes(j_str.c_str(), &root_text.first,
                               root_text.second.c_str());

        if (res.empty()) {
          continue;
        } // if pos is not found

        for (int pos : res) {
          if (DEBUG_LEVEL > 3) {
            printf("\t\tj_str: %s pos: %d\n", j_str.c_str(), pos);
          }

          pos = pos + j_str.length() - 1;

          if (j == 0) {
            w_matrix[j][pos] = true;
            inc_j = true;
            continue;
          }

          if (can_extend(eds_w, w_matrix, pos, j_str, j, i)) {
            size_t pos_in_n = eds_w.str_offsets[i][0].start + pos;
            w_matrix[j][pos_in_n] = true;
            inc_j = true;

            if (DEBUG_LEVEL > 4) {
              printf("\t\tpos %d \tpos_in_n: %lu new value: %d \n", pos,
                     pos_in_n, w_matrix[j][pos_in_n]);
            }
          }
        }
      }

      if (inc_i) {
        ++i;
      }

      if (inc_j) {
        ++j;
      }

      // give up early we cant extend any degenerate letter
      if (!inc_j && !inc_i) {
        return false;
      }
  }

    // computing accepting states
    std::vector<std::size_t> w_ends = compute_str_ends(eds_w, len_w - 1);
    std::vector<std::size_t> q_ends = compute_str_ends(eds_q, len_q - 1);

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

  ed_string = "{AT,TC}{ATC,T}";
  parse_ed_string(ed_string);

  ed_string = "{AT,TC}{ATC,}";
  parse_ed_string(ed_string);

  ed_string = "ACTGAC{AT,,TC}AGG{,ATC,}CT";
  parse_ed_string(ed_string);

  ed_string = "{AT,TC}{ATC,T}";

  EDS eds = parse_ed_string(ed_string);

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

void test_lacks_intersect() {
  std::string ed_string_w = "{AT,TC}{ATC,T}";
  std::string ed_string_q = "{TC,G}{CG,G}";

  EDS eds_w = parse_ed_string(ed_string_w);
  EDS eds_q = parse_ed_string(ed_string_q);

  IS_TRUE(!intersect(eds_w, eds_q));
}
