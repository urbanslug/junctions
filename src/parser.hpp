#ifndef PARSER_H
#define PARSER_H

#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ostream>
#include <set>
#include <unordered_set>
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>

#include "./utils.hpp"

namespace parser {
// ----
// IO
// ---
std::string read_eds(std::string &file_path) {
  std::string line, eds_string;

  std::ifstream myfile(file_path);
  if (myfile.is_open()) {
    while (getline(myfile, line)) {
      eds_string.append(line);
    }
    myfile.close();
  } else {
    std::cerr << "[files::read_eds] Error parsing: " << file_path << std::endl;
    exit(1);
  }

  return eds_string;
}

std::vector<std::string> read_msa(std::string &file_path) {
  std::string line;
  std::vector<std::string> msa_data;
  // TODO: remove unused
  // bool has_header = false;

  int line_number = 0;

  std::ifstream myfile(file_path);
  if (myfile.is_open()) {
    while (getline(myfile, line)) {
      if (line_number % 2 == 0) {
        if (line[0] != '>') {
          std::cerr << "[files::read_msa] no header file " << file_path
                    << " in line " << line_number << std::endl;
          exit(1);
        }

        ++line_number;
        continue;
      }

      ++line_number;
      msa_data.push_back(line);
    }
    myfile.close();
  } else {
    std::cerr << "[files::read_msa] Error parsing: " << file_path << std::endl;
    exit(1);
  }

  return msa_data;
}

EDS parse_ed_string(std::string &ed_string, core::Parameters &parameters) {
  if (parameters.verbosity > 1) {
    std::cerr << utils::indent(1) << "[parser::parse_ed_string]"
              << std::endl;
  }

  std::vector<std::string> degenerate_letter_data;
  std::vector<degenerate_letter> ed_string_data;

  degenerate_letter letter;
  letter.has_epsilon = false;

  // TODO: remove unused
  // size_t len;

  std::string str;
  char prev_char = '\0'; // TODO: is this the best way to initialize?

  size_t size = 0;
  for (auto ch : ed_string) {
    // printf("prev char %c ch %c\n", prev_char, ch);

     if (ch == '{' || ch == '}') {
      if (str.empty() && prev_char != ',') { continue; }

      if (str.empty() && (prev_char == ',' || prev_char == '{')) {
        letter.has_epsilon = true;
      } else {
        degenerate_letter_data.push_back(str);
      }

      // degenerate_letter_data.push_back(str);
      letter.data = degenerate_letter_data;
      ed_string_data.push_back(letter);
      degenerate_letter_data.clear();
      letter.has_epsilon = false;
      str.clear();
    } else if (ch == ',') {
      if (str.empty()) {
        letter.has_epsilon = true;
        continue;
      } else {
        degenerate_letter_data.push_back(str);
      }
      //letter.has_epsilon = false;
      str.clear();
    } else if (ch == 'A' || ch == 'C' || ch == 'T' || ch == 'G') {
      ++size;
      str.push_back(ch);
    } else {
      printf("error found char %c at pos\n", ch);
    }

    prev_char = ch;
  }

  if (!str.empty()) {
    degenerate_letter_data.push_back(str);
    letter.data = degenerate_letter_data;
    ed_string_data.push_back(letter);
  }

  size_t index = 0;
  std::unordered_set<size_t> stops, starts;
  std::vector<std::vector<span>> str_offsets;
  std::vector<std::vector<slicex>> str_slices;

  int eps_count = 0;
  int strs = 0;
  size_t str_start = 0;

  for (size_t i = 0; i < ed_string_data.size(); i++) {
    std::vector<span> letter_offsets;
    std::vector<std::string> i_strings = ed_string_data[i].data;
    span s;

    std::vector<slicex> letter_slices;
    letter_slices.reserve(i_strings.size());
    slicex sl;
    str_start = 0;

    for (auto str : i_strings) {
      ++strs;

      sl.start = str_start;
      sl.length = str.length();
      letter_slices.push_back(sl);
      str_start += str.length();

      s.start = index;
      starts.insert(s.start);
      index += str.length();
      s.stop = index - 1;
      stops.insert(s.stop);
      letter_offsets.push_back(s);
    }

    if (ed_string_data[i].has_epsilon) {
      ++eps_count;

      letter_slices.push_back(slicex{.start = str_start, .length = 0});

      s.start = index;
      s.stop = index++;
      starts.insert(s.start);
      stops.insert(s.stop);
      letter_offsets.push_back(s);
    }

    str_slices.push_back(letter_slices);
    str_offsets.push_back(letter_offsets);
  }

  // TODO: not counting epsilons in size breaks linearize
  // size = str_offsets.back().back().stop + 1;

  // std::cerr << "eps count: " << eps_count << std::endl;

  EDS e;
  e.data = ed_string_data;
  e.length = ed_string_data.size();
  e.size = size;
  e.epsilons = eps_count;
  e.m = eps_count + strs;
  e.str_offsets = str_offsets;
  e.str_slices = str_slices;
  e.starts = starts;
  e.stops = stops;

  return e;
}

LinearizedEDS linearize(EDS &eds) {
  if (utils::DEBUG_LEVEL > 3) { std::cerr << "[utils::parser::foo]" << std::endl; }

  size_t size = eds.str_offsets.back().back().stop + 1;

  std::vector<std::vector<int>> prev_m;
  prev_m.reserve(size);

  std::string chars_vec;
  chars_vec.reserve(size);

  std::vector<int> prev_char;
  prev_char.reserve(100);
  for (size_t k = 0; k < size; k++) {
    prev_char.push_back(k-1);
    prev_m.push_back(prev_char);
    prev_char.clear();
  }

  // Handle the first D letter
  for (auto sp : eds.str_offsets.front()) {
    prev_m[sp.start].clear();
  }

  degenerate_letter d_letter = eds.data.front();
  for (auto st : d_letter.data) {
    chars_vec.insert(chars_vec.end(), st.begin(), st.end());
  }

  if (d_letter.has_epsilon) {
    chars_vec.push_back('*');
  }

  // Handle the rest of the EDT
  for (size_t i=1; i < eds.length; i++) {
    std::vector<span> spans_curr = eds.str_offsets[i];
    std::vector<span> spans_prev = eds.str_offsets[i-1];

    prev_char.clear();
    for (auto sp_prev : spans_prev) {
      prev_char.push_back(sp_prev.stop);
    }

    for (auto sp: spans_curr) {
      int idx = sp.start;
      prev_m[idx] = prev_char;
    }

    degenerate_letter d_letter = eds.data[i];
    for (auto st : d_letter.data) {
      chars_vec.insert(chars_vec.end(), st.begin(), st.end());
    }

    if (d_letter.has_epsilon) {
      chars_vec.push_back('*');
    }
  }

  if (false) {
    printf("%s\n", chars_vec.c_str());

    for (auto i : prev_m) {
      printf("[");
      for (auto j : i) { printf("%d , ", j); }
      printf("], ");
    }
    printf("\n");
  }

  LinearizedEDS l;
  l.prev_chars = prev_m;
  l.str = chars_vec;

  return l;
}

std::string msa_to_eds(string_vec &msa) {
  int last_col_idx = msa.front().length(); // rename to col count
  int last_row_idx = msa.size() ; // rename to row count

  // printf("last row idx: %d last col idx: %d\n", last_row_idx, last_col_idx);

  // find similar regions
  bool variable_region = true;
  std::pair<int,int> span;
  std::vector<std::pair<int,int>> spans;

  for (int col_idx = 0; col_idx < last_col_idx; col_idx++) {
    // printf("col idx: %d\n", col_idx);
    for (int row_idx = 0; row_idx < last_row_idx - 1; row_idx++) {
      // printf("\trow idx: %d ", row_idx);
      // printf("%c %c ", msa[row_idx][col_idx] , msa[row_idx+1][col_idx]);
      // printf("variable region: %d\n", variable_region);

      if (msa[row_idx][col_idx] != msa[row_idx+1][col_idx] && !variable_region) {
        variable_region = true;
        span = std::make_pair(span.first, col_idx-1);
        spans.push_back(span);
        break;
      }

      // could be combined with above
      if (col_idx == last_col_idx-1) {
        span = std::make_pair(span.first, col_idx);
        spans.push_back(span);
        break;
      }

      if (row_idx == last_row_idx - 2 &&
          msa[row_idx][col_idx] == msa[row_idx+1][col_idx] &&
          variable_region) {
        variable_region = false;
        span = std::make_pair(col_idx, -1);
        // span.a = col_idx;
      }
    }
  }

  if (false) {
    printf("Spans: [");
    for (auto span : spans) {
      printf("(%d, %d), ", span.first, span.second);
    }
    printf("]\n");
  }

  // -----
  //
  // -----
  std::vector<std::set<std::string>> raw_edt;
  std::set<std::string> d_letter;
  std::string s;

  int spans_idx = 0;
  bool increment_spans_idx;
  for  (int col_idx = 0; col_idx < last_col_idx; col_idx++) {
    // printf("col idx: %d, spans idx %d \n", col_idx, spans_idx);
    increment_spans_idx = false;

    // start in variable region
    if (col_idx == 0 && spans[spans_idx].first > 0) {
      int len = spans[spans_idx].first;

      for (int row_idx = 0; row_idx < last_row_idx; row_idx++) {
        for (char ch: msa[row_idx].substr(0, len)) {
          if (ch != '-') {
            s.push_back(ch);
          }
        }
        d_letter.insert(s);
        s.clear();
      }
      if (!d_letter.empty()) {
        raw_edt.push_back(d_letter);
      }

      d_letter.clear();
    }

    // variable region
    if (spans_idx > 0 && col_idx == spans[spans_idx].first) {
      // if (spans_idx == 0) { continue; }

      int len = ((spans[spans_idx].first - 1) - spans[spans_idx-1].second);

      for (int row_idx = 0; row_idx < last_row_idx; row_idx++) {
        for (char ch: msa[row_idx].substr(spans[spans_idx-1].second + 1, len)) {
          if (ch != '-') {
            s.push_back(ch);
          }
        }
        d_letter.insert(s);
        s.clear();
      }
      if (!d_letter.empty()) {
        raw_edt.push_back(d_letter);
      }
      d_letter.clear();
      increment_spans_idx = true;
    }

    // collapsible region
    if (col_idx == spans[spans_idx].first) {
      int len = (spans[spans_idx].second - spans[spans_idx].first) + 1;
      for (char ch: msa.front().substr(spans[spans_idx].first, len)) {
        if (ch != '-') {
          s.push_back(ch);
        }
      }
      d_letter.insert(s);
      if (!d_letter.empty()) {
        raw_edt.push_back(d_letter);
      }
      s.clear();
      d_letter.clear();
      increment_spans_idx = true;
    }


    // end in variable region
    if (col_idx == (last_col_idx - 1) && spans.back().second < col_idx) {
      // printf("spans idx sc %d \n", spans.back().second);
      for (int row_idx = 0; row_idx < last_row_idx; row_idx++) {
        for (char ch: msa[row_idx].substr(spans.back().second + 1)) {
          if (ch != '-') {
            s.push_back(ch);
          }
        }
        d_letter.insert(s);
        s.clear();
      }
      if (!d_letter.empty()) {
        raw_edt.push_back(d_letter);
      }
      d_letter.clear();
    }


    if (increment_spans_idx) {
      ++spans_idx;
    }
  }

  std::string ed_string;

  if (true) {
    // printf("raw_edt: ");
    for (auto d_letter : raw_edt) {
      if (d_letter.size() < 2) {
        ed_string.append(*d_letter.begin());
        // printf("%s", d_letter.begin()->c_str());
      } else {
        ed_string.push_back('{');
        // printf("{");
        for (auto str = d_letter.begin(); str != d_letter.end(); str++) {
          if (str->length() > 0 ) {
            ed_string.append(*str);
            // printf("%s", str->c_str());
          } else {
            // printf("e,");
          }
          if (std::next(str) != d_letter.end()) {
            ed_string.push_back(',');
            // printf(",");
          }
        }
        ed_string.push_back('}');
        // printf("}");
      }
    }
    // printf("\n");
  }

  // return raw_edt;
  return ed_string;
}
}

#endif
