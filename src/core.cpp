#include <ostream>
#include <tuple>

#include "core.hpp"




ll nwd(ll a, ll b) { return !b ? a : nwd(b, a % b); }
template <class T> inline T sqr(const T &a) { return a * a; }

VS parse(std::string s) {
  std::string a;
  VS wyn;
  REP(i, (int)s.size())
  if (s[i] != ' ')
    a += s[i];
  else if (!a.empty()) {
    wyn.PB(a);
    a = "";
  }
  if (!a.empty())
    wyn.PB(a);
  return wyn;
}

int toi(char ch) { return int(ch) - int('0'); }

int chg(char ch) { return int(ch) - int('a'); }

int los(int m) { return (int)((double)m * (rand() / (RAND_MAX + 1.0))); }

namespace junctions {

/* -> */
bool logical_implication(bool p, bool q) { return !p || q; }

/* <-> */
bool double_implication(bool a, bool b) { return !(a && !b) && !(!a && b); }


std::string indent(int level) {
  std::string repeat;
  for (int i = 0; i < level; i++) {
    repeat += "\t";
  }
  return repeat;
}

// TODO: combine with loop
// print (debug) info
void print_eds_info(EDS const &w, EDS const &q) {
std::cerr << "Input info {"
          << std::endl << indent(1)
          << "N"  << junctions::unicode_sub_1 << ": "<< w.size
          << " m" << junctions::unicode_sub_1 << ": " << w.m
          << " n" << junctions::unicode_sub_1 << ": " << w.length
          << std::endl << indent(1)
          << "N"  << junctions::unicode_sub_2 << ": " << q.size
          << " m" << junctions::unicode_sub_2 << ": " << q.m
          << " n" << junctions::unicode_sub_2 << ": " << q.length
          << std::endl << "}" << std::endl;
};

/**
 * concatenate a vector of strings with a given characters interspersing them
 *
 *
 * @param[in]  v vector of input strings to concatenate
 * @param[in]  c the character to have in between the strings
 * @param[out] s the resulting concatenated string
 */
void join(const std::vector<std::string> &v, char c, std::string &s) {
s.clear();

for (std::vector<std::string>::const_iterator p = v.begin(); p != v.end();
     ++p) {
    s += *p;
    if (p != v.end() - 1)
      s += c;
}
}

// TODO deprecated
/**
 *
 * slices exist in l
 * @param[in]  queries           queries
 * @param[in]  text              text
 * @param[out] candidate_matches matches_found in the context of the degenerate
 * letter and not N
 */
void perform_matching(std::vector<std::string> const &queries,
                      std::vector<slicex> const &txt_slices,
                      std::pair<STvertex, std::string> *text,
                      std::vector<junctions::match> *candidate_matches,
                      core::Parameters const &parameters) {
if (parameters.verbosity > 2) {
    std::cerr << indent(1) << "DEBUG, [graph::match]" << std::endl;
  }

  std::vector<junctions::extended_match> match_positions;

  for (int qry_str_idx = 0; qry_str_idx < queries.size(); qry_str_idx++) {
    std::string qry_str = queries[qry_str_idx];
    match_positions = FindEndIndexes(qry_str.c_str(), &text->first, text->second.c_str());
    
    for (auto match_pos : match_positions) {

      candidate_matches->push_back(
        junctions::match{.query_str_index = (int)qry_str_idx,
                         .text_str_index = match_pos.str_idx,
                         .text_char_index = match_pos.chr_idx ,
                         .match_length = match_pos.match_length,
                         .beyond_txt = match_pos.beyond_text,
                         .str = qry_str.substr(0, match_pos.match_length)});
    }
  }
}

/**
 *
 * slices exist in l
 * @param[in]  queries           queries
 * @param[in]  text              text
 * @param[out] candidate_matches matches_found in the context of the degenerate
 * letter and not N
 */
void perform_matching(std::vector<std::string> const &queries,
                      std::pair<STvertex, std::string> *text,
                      std::vector<junctions::match> *candidate_matches,
                      core::Parameters const &parameters) {

  std::vector<junctions::extended_match> match_positions;

  for (int qry_str_idx = 0; qry_str_idx < queries.size(); qry_str_idx++) {
    std::string qry_str = queries[qry_str_idx];
    match_positions = FindEndIndexes(qry_str.c_str(), &text->first, text->second.c_str());
    
    for (auto match_pos : match_positions) {

      candidate_matches->push_back(
        junctions::match{.query_str_index = (int)qry_str_idx,
                         .text_str_index = match_pos.str_idx,
                         .text_char_index = match_pos.chr_idx ,
                         .match_length = match_pos.match_length,
                         .beyond_txt = match_pos.beyond_text,
                         .str = qry_str.substr(0, match_pos.match_length)});
    }
  }
}


// Extended match
// ----------

bool operator==(const junctions::extended_match &lhs, const junctions::extended_match &rhs) {
  return std::tie(lhs.beyond_text, lhs.match_length, lhs.str_idx, lhs.chr_idx) ==
         std::tie(lhs.beyond_text, lhs.match_length, lhs.str_idx, lhs.chr_idx);
}

std::ostream &operator<<(std::ostream &os, const junctions::extended_match &r) {
  os << "beyond text: " << r.beyond_text
     << " match length: " << r.match_length
     << " str idx: " << r.str_idx
     << " char idx: " << r.chr_idx;
  return os;
}

// match locus
// -----------
bool operator<(const junctions::match_locus &lhs,
               const junctions::match_locus &rhs) {
  return std::tie(lhs.string_index, lhs.char_index) <
         std::tie(rhs.string_index, rhs.char_index);
}

bool operator==(const junctions::match_locus &lhs,
                const junctions::match_locus &rhs) {
  return std::tie(lhs.string_index, lhs.char_index) ==
         std::tie(rhs.string_index, rhs.char_index);
}

// match
// -----
std::ostream &operator<<(std::ostream &os, const junctions::match &m) {
  os << "qry str idx: " << m.query_str_index
     << " txt str idx: " << m.text_str_index
     << " txt char idx: " << m.text_char_index
     << " match length: " << m.match_length
     << " beyond txt: " << m.beyond_txt
     << " str: " << m.str;
  return os;
}


// graph slice
// ------

std::ostream &operator<<(std::ostream &os, const junctions::graph_slice &s) {
  os << "Graph slice {"
     << "\ntxt start: " << s.txt_start << "\nqry start: " << s.qry_start
     << "\nlen: " << s.len << "\nstr: " << s.str << "\n}" << std::endl;
  return os;
}
}

// ed string
std::ostream &operator<<(std::ostream &os, const core::ed_string &value) {
  os << (value == core::ed_string::q ? "Q" : "W");
  return os;
}

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
