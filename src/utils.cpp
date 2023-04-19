#include <iostream>

#include "utils.hpp"



std::string utils::indent(int level) {
  std::string repeat;
  for (int i = 0; i < level; i++) {
    repeat += "\t";
  }
  return repeat;
}


namespace utils {
void print_newline() { std::cerr << std::endl; }

void print_degenerate_letter(degenerate_letter &d_letter) {
  std::vector<std::string> degenerate_letter_data = d_letter.data;
  std::cout << "{";
  for (auto str = degenerate_letter_data.begin();
       str < degenerate_letter_data.end();) {
    std::cout << *str;
    str++;

    if (str == degenerate_letter_data.end()) {
      if (d_letter.has_epsilon) {
        std::cout << ", \u03B5";
      }

      std::cout << "}";
      continue;
    } else {
      std::cout << ", ";
    }
  }
}

void print_edt_data(std::vector<degenerate_letter> & ed_string_data) {
  for (auto degenerate_letter : ed_string_data) {
    print_degenerate_letter(degenerate_letter);
  }
  std::cout << std::endl;
}

void print_edt(EDS & ed_string) {
  printf("size: %lu length: %lu\n", ed_string.size, ed_string.length);

  for (auto off : ed_string.str_offsets) {
    printf("{");
    for (auto sp : off) {
      printf("(%lu, %lu)", sp.start, sp.stop);
    }
    printf("}");
  }

  printf("\n");

    print_edt_data(ed_string.data);
}

void print_edt_range(EDS & eds, int start, int stop) {
  for (int i = start; i <= stop; i++) {
    print_degenerate_letter(eds.data[i]);
  }
}

void print_str_vec(std::vector<std::string> & degenerate_letter) {
  for (auto str : degenerate_letter) {
    std::cout << str << " ";
  }
}

void print_str_vec(std::set<std::string> & degenerate_letter) {
  for (auto str : degenerate_letter) {
    std::cout << str;
  }
}

// TODO: remove
void print_d_letter(std::vector<std::string> & degenerate_letter) {
  for (auto str : degenerate_letter) {
    std::cout << str << ",";
  }
}

// TODO: remove
void print_d_letter(std::set<std::string> & degenerate_letter) {
  for (auto str = degenerate_letter.begin(); str != degenerate_letter.end();
       str++) {
    std::cout << *str;
    if (std::next(str) != degenerate_letter.end() || str->empty()) {
      std::cout << ",";
    }
  }
}

void print_vec(std::vector<std::size_t> const &v) {
  for (auto str = v.begin(); str < v.end(); str++) {
    std::cout << *str << " ";
  }
}

void print_matrix(boolean_matrix const &m) {
  for (auto row : m) {
    for (auto col : row) {
      std::cout << col;
      // printf("%d ", col);
    }
    printf("\n");
  }
}

boolean_matrix gen_matrix(size_t rows, size_t cols) {
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
std::vector<std::size_t> compute_str_ends(EDS & eds, size_t letter) {
  if (DEBUG_LEVEL > 1) {
    printf("[cpp::main::compute_str_ends]\n");
  }

  std::vector<std::size_t> stops;
  for (auto s : eds.str_offsets[letter]) {
    stops.push_back(s.stop);
  }
  return stops;
}

// compute the accepting state indexes
// the ends of the strings in the last degenerate letter
std::vector<std::size_t> compute_accepting_states(EDS & eds) {
  return compute_str_ends(eds, eds.length - 1);
}

std::vector<std::size_t> compute_str_starts(EDS & eds, size_t letter) {
  if (DEBUG_LEVEL > 1) {
    printf("[cpp::main::compute_str_ends]\n");
  }

  std::vector<std::size_t> starts;
  for (auto s : eds.str_offsets[letter]) {
    starts.push_back(s.start);
  }
  return starts;
}

// is at least one the strings in the letter_idx index of the eds given
// matched? it is matched is at least one of its indexes is true in the dp
// matrix
bool is_letter_matched(
    EDS & eds,         // the eds we are checking
    int letter_idx,    // has any string in this d letter been matched
    matrix &dp_matrix, // matrix that tracks matches in the eds
    int row_idx        // row in dp_matrix
) {
  std::vector<span> &letter_span = eds.str_offsets[letter_idx];

  for (auto sp : letter_span) {
    if (dp_matrix[row_idx][sp.stop]) {
      return true;
    }
  }

  return false;
};

// is at least one the strings in the letter_idx index of the eds given
// matched? it is matched is at least one of its indexes is true in the dp
// matrix
bool is_prev_letter_matched(
    EDS & eds,         // the eds we are checking
    int letter_idx,    // has any string in this d letter been matched
    matrix &dp_matrix, // matrix that tracks matches in the eds
    int row_idx        // row in dp_matrix
) {
  if (row_idx == 0) {
    return true;
  }

  return is_letter_matched(eds, letter_idx, dp_matrix, row_idx - 1);
};

bool is_prefix_matched(
    EDS & eds,          //
    matrix & dp_matrix, //
    int pos,            // the position in which our match started
    size_t row,         // the letter whose chars we are matching in
    size_t comp         // the letter whose strings we are matching against
) {
  // TODO: should this be a one liner?
  // can we extend from within this current degenerate letter?
  size_t letter_start_in_size = eds.str_offsets[comp].front().start;
  size_t match_start_in_size = pos + letter_start_in_size;
  return dp_matrix[row][match_start_in_size - 1];
}

// filter results from ST
bool is_match_valid(std::vector<span> & spans, int letter_idx,
                    std::size_t query_length, int match_position,
                    int letter_start_in_n) {
  for (auto sp : spans) {
    int k = match_position;
    if (sp.stop >= k && (k + query_length - 1) > sp.stop) {
      return false;
    }
  }

  return true;
}

bool is_pos_at_start(std::vector<span> spans, int pos_in_n) {
  for (auto sp : spans) {
    if (sp.start == pos_in_n) {
      return true;
    }
  }

  return false;
}
}
