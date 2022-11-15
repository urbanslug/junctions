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

    printf("\t\t len_q %d len_w %d\n", len_q, len_w);

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

    auto is_letter_matched = [](EDS &eds, matrix dp_matrix, int letter_idx,
                                std::vector<int> row_idx_range) -> bool {
      std::vector<span> letter_span = eds.str_offsets[letter_idx];

      for (int row_idx : row_idx_range) {
        for (auto sp : letter_span) {
          if (dp_matrix[row_idx][sp.stop]) {
            return true;
          }
        }
      }

      return false;
    };

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
      size_t start = pos;

      start += letter_start_in_n;

      // TODO: is it better to call  compute_str_starts ?
      // is start in the start of a degenerate letter?
      bool check_prev_letter = false;
      for (auto sp : eds.str_offsets[comp]) {
        // printf("(%lu, %lu)", s.start, start);
        if (sp.start == start) {
          check_prev_letter = true;
          break;
        }
      }

      // printf("\t\t\tletter start: %lu pos %d curr start %lu prev : %d\n",
      //       letter_start_in_n, pos, start, check_prev_letter);

      if (check_prev_letter) {
        // check whether any of the previous letter ends are true
        for (auto sp : eds.str_offsets[comp - 1]) {
          
          if (dp_matrix[curr][sp.stop]) {
            return true;
          }
          
          
        }
      } else {
        // we are extending within the same degenerate letter i
        if (dp_matrix[curr][start - 1]) {
          return true;
        }
      }

      

      return false;
    };

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
      root_text = q_suffix_trees[j];

      if (DEBUG_LEVEL > 3) {
        printf("\tj text: %s\n", root_text.second.c_str());
      }

      if (eds_w.data[i].has_epsilon) {
        std::vector<span> letter_span = eds_w.str_offsets[i];
        int epsilon_idx = letter_span.back().stop;

        if (j == 0) {
          w_matrix[j][epsilon_idx] = true;
          inc_i = true;
        } else {
          std::vector<int> row_idx_range;
          row_idx_range.push_back(j);
          row_idx_range.push_back(j - 1);

          if ((is_letter_matched(eds_w, w_matrix, i - 1, row_idx_range))) {
            w_matrix[j][epsilon_idx] = true;
            inc_i = true;
          }
        }
      }

      for (auto i_str : eds_w.data[i].data) {
        // if (DEBUG_LEVEL > 3) { printf("\t\ti_str: %s\n", i_str.c_str()); }

        vector<int> res = FindEndIndexes(i_str.c_str(), &root_text.first, root_text.second.c_str());

        if (res.empty()) {
          continue;
        } // if pos is not found

        for (int pos: res) {
          if (DEBUG_LEVEL > 3) {
            printf("\t\ti_str: %s pos: %d\n", i_str.c_str(), pos);
          }

          if (i == 0) {
            int end_idx = pos + i_str.length() - 1;
            q_matrix[i][end_idx] = true;
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

      if (eds_q.data[j].has_epsilon) {
        std::vector<span> letter_span = eds_q.str_offsets[j];
        int epsilon_idx = letter_span.back().stop;


        if (i == 0) {
          q_matrix[i][epsilon_idx] = true;
          inc_j = true;
        } else {
          std::vector<int> row_idx_range;
          row_idx_range.push_back(i);
          row_idx_range.push_back(i - 1);

          if ((is_letter_matched(eds_q, q_matrix, j - 1, row_idx_range))) {
            q_matrix[i][epsilon_idx] = true;
            inc_j = true;
          }
        }
      }

      std::vector<std::string> j_strings =  eds_q.data[j].data;


      for (auto j_str : j_strings) {

        vector<int> res = FindEndIndexes(j_str.c_str(), &root_text.first,
                               root_text.second.c_str());

        if (res.empty()) {
          continue;
        } // if pos is not found

        if (DEBUG_LEVEL > 3) {
          printf("\t\tj_str: %s\n", j_str.c_str());
        }

        for (int pos : res) {

          if (DEBUG_LEVEL > 3) {
            printf("\t\t\tpos: %d\n", pos);
          }

          // filter results from ST
          bool invalid_pos = false;
          int letter_start_in_n = eds_w.str_offsets[i].front().start;
          for (auto sp : eds_w.str_offsets[i]) {
            int k = letter_start_in_n + pos;
            if (sp.stop >= k && (k + j_str.length() -1)  > sp.stop) {
              invalid_pos = true;
            }
          }

          if (invalid_pos) {continue;}

          if (j == 0) {
            int end_idx = pos + j_str.length() - 1;
            w_matrix[j][end_idx] = true;
            inc_j = true;
            continue;
          }

          if (can_extend(eds_w, w_matrix, pos, j_str, j, i)) {
            size_t pos_in_n = eds_w.str_offsets[i][0].start + pos;
            w_matrix[j][pos_in_n] = true;
            inc_j = true;

            if (DEBUG_LEVEL > 4) {
              printf("\t\t\tset j %lu \tpos_in_n: %lu\n", j, pos_in_n);
            }
          }
        }
      }

      if (inc_i) {
        ++i;
        if (i < len_w) {
          for (int k = 0; k < size_q; k++) {
            q_matrix[i][k] = q_matrix[i - 1][k];
          }
        }
      }

      if (inc_j) {
        ++j;
        if (j < len_q) {
          for (int k = 0; k < size_w; k++) {
            w_matrix[j][k] = w_matrix[j - 1][k];
          }
        }
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

  std::cout << accept_w << " " << accept_q << "\n";

  return accept_w && accept_q;
  }

int main() {
  test_handle_epsilon();
  // test_contains_intersect();
  // test_lacks_intersect();
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
  print_edt(eds);

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
  std::string ed_string_w;
  std::string ed_string_q;
  EDS eds_w;
  EDS eds_q;

  ed_string_w = "{AT,TC}{ATC,T}";
  ed_string_q = "TC{,G}{CT,T}";

  eds_w = parse_ed_string(ed_string_w);
  eds_q = parse_ed_string(ed_string_q);

  IS_TRUE(intersect(eds_w, eds_q));

  /*
  ed_string_w = "{AT,TC}{ATC,T}";
  ed_string_q = "{,G}{CT,T}";

  eds_w = parse_ed_string(ed_string_w);
  eds_q = parse_ed_string(ed_string_q);

  IS_TRUE(intersect(eds_w, eds_q));

  ed_string_w = "{AT,TC}{ATC,}";
  ed_string_q = "{TC,G}{CT,T}";

  eds_w = parse_ed_string(ed_string_w);
  eds_q = parse_ed_string(ed_string_q);

  IS_TRUE(intersect(eds_w, eds_q));

  ed_string_w = "{AT,TC}{ATC,}";
  ed_string_q = "{,G}{CT,T}";

  eds_w = parse_ed_string(ed_string_w);
  eds_q = parse_ed_string(ed_string_q);

  IS_TRUE(intersect(eds_w, eds_q));
  */
}

void test_lacks_intersect() {
  std::string ed_string_w = "{AT,TC}{ATC,T}";
  std::string ed_string_q = "{TC,G}{CG,G}";

  EDS eds_w = parse_ed_string(ed_string_w);
  EDS eds_q = parse_ed_string(ed_string_q);

  IS_TRUE(!intersect(eds_w, eds_q));
}
