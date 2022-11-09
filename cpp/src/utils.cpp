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


