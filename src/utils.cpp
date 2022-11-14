#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>

#include "./core.cpp"

void print_degenerate_letter(std::vector<std::string> &degenerate_letter) {
  std::cout << "{";
  for (auto str = degenerate_letter.begin(); str < degenerate_letter.end();) {
    if (str->empty()) {
      std::cout << "\u03B5";
    } else {
      std::cout << *str;
    }

    str++;

    if (str == degenerate_letter.end()) {
      std::cout << "}";
      continue;
    } else {
      std::cout << ", ";
    }
  }
}


void print_edt_data(ed_string_data &ed_string) {
  for (auto degenerate_letter : ed_string) {
    print_degenerate_letter(degenerate_letter);
  }
  std::cout << std::endl;
}

void print_edt(EDS &ed_string) {
  printf("size: %lu length: %lu\n", ed_string.size , ed_string.length);

  for (auto off: ed_string.str_offsets) {
    printf("{");
    for (auto sp: off) {
      printf("(%lu, %lu)", sp.start, sp.stop);
    }
    printf("}");
  }

  printf("\n");

  print_edt_data(ed_string.data);
}

// TODO: remove
void print_str_vec(std::vector<std::string> &degenerate_letter) {
    for (auto str = degenerate_letter.begin(); str < degenerate_letter.end(); str++) {
      std::cout << *str << " ";
    }
}

void print_vec(std::vector<std::size_t> const &v) {
  for (auto str = v.begin(); str < v.end(); str++) {
    std::cout << *str << " ";
  }
}

void print_matrix(matrix const  &m) {
  for (auto row : m) {
    for (auto col: row) {
      std::cout << col;
      // printf("%d ", col);
    }
    printf("\n");
  }
  
}




EDS parse_ed_string(std::string &ed_string) {
  if (DEBUG_LEVEL > 3) {
    printf("[cpp::main::parser]\n");
  }

  ed_string_data data;
  degenerate_letter letter;

  size_t size, len;
  size = 0;

  std::string str;
  char prev_char;


  for (auto ch : ed_string) {
    // printf("prev char %c ch %c\n", prev_char, ch);

    if (ch == '{' || ch == '}') {
      if (str.empty() && prev_char != ',') { continue; }
      letter.push_back(str);
      data.push_back(letter);
      letter.clear();
      str.clear();
    } else if (ch == ',') {
      letter.push_back(str);
      str.clear();
    } else if (ch == 'A' || ch == 'C' || ch == 'T' || ch == 'G') {
      str.push_back(ch);
      ++size;
    } else {
      printf("error found char %c at pos\n", ch);
    }

    prev_char = ch;
  }

  if (!str.empty()) {
    letter.push_back(str);
    data.push_back(letter);
  }

  // printf("len %lu\n", data.size());



  // print_edt(data);

  std::vector<std::vector<span>> str_offsets;
  size_t index = 0;
  for (size_t i = 0; i < data.size(); i++) {

    std::vector<span> letter_offsets;
    std::vector<std::string> i_strings = data[i];
    span s;

    for (auto str : i_strings) {
      s.start = index;
      index += str.length();
      s.stop = index - 1;
      letter_offsets.push_back(s);
    }

    str_offsets.push_back(letter_offsets);
  }

  EDS e;
  e.data = data;
  e.length = data.size();
  e.size = size;
  e.str_offsets = str_offsets;

  return e;
}

std::vector<std::vector<bool>> gen_matrix(size_t rows, size_t cols) {
  if (DEBUG_LEVEL > 3) {
    printf("[cpp::main::gen_matrix]\n");
  }

  // rows
  std::vector<bool> row(cols);
  fill(row.begin(), row.end(), false);

  // the matrix
  std::vector<std::vector<bool>> actual_matrix(rows);

  fill(actual_matrix.begin(), actual_matrix.end(), row);


  return actual_matrix;
}

/*
  compute string ends indexes (0 indexed) given an eds and letter
  within N
 */
std::vector<std::size_t> compute_str_ends(EDS &eds, size_t letter) {
  if (DEBUG_LEVEL > 1) { printf("[cpp::main::compute_str_ends]\n");  }

  std::vector<std::size_t> stops;
  for (auto s : eds.str_offsets[letter]) { stops.push_back(s.stop); }
  return stops;
}

std::vector<std::size_t> compute_str_starts(EDS &eds, size_t letter) {
  if (DEBUG_LEVEL > 1) { printf("[cpp::main::compute_str_ends]\n"); }

  std::vector<std::size_t> starts;
  for (auto s : eds.str_offsets[letter]) { starts.push_back(s.start); }
  return starts;
}
