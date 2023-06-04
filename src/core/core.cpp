#include "./core.hpp"
#include <vector>


namespace n_core {
Parameters::Parameters() {
}

bool_matrix gen_matrix(std::size_t rows, std::size_t cols) { 
  // rows
  std::vector<bool> row(cols);
  fill(row.begin(), row.end(), false);

  // the matrix
  std::vector<std::vector<bool>> actual_matrix(rows);

  fill(actual_matrix.begin(), actual_matrix.end(), row);

  return actual_matrix;
}

// ed string
// ---------
std::ostream &operator<<(std::ostream &os, const ed_string &value) {
  os << (value == ed_string::q ? "Q" : "W");
  return os;
}

  // TODO remove
// match locus
// -----------
bool operator<(const match_locus &lhs,
               const match_locus &rhs) {
  return std::tie(lhs.string_index, lhs.char_index) <
         std::tie(rhs.string_index, rhs.char_index);
}

bool operator==(const match_locus &lhs,
                const match_locus &rhs) {
  return std::tie(lhs.string_index, lhs.char_index) ==
         std::tie(rhs.string_index, rhs.char_index);
}
// TODO remove
// Extended match
// ----------

bool operator==(const extended_match &lhs,
                const extended_match &rhs) {
  return std::tie(lhs.beyond_text, lhs.match_length, lhs.str_idx,
                  lhs.chr_idx) ==
         std::tie(lhs.beyond_text, lhs.match_length, lhs.str_idx, lhs.chr_idx);
}

std::ostream &operator<<(std::ostream &os, const extended_match &r) {
  os << "beyond text: " << r.beyond_text << " match length: " << r.match_length
     << " str idx: " << r.str_idx << " char idx: " << r.chr_idx;
  return os;
}

} // namespace core

namespace n_junctions {

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

std::string indent(int level) {
  std::string repeat;
  for (int i = 0; i < level; i++) {
    repeat += "\t";
  }
  return repeat;
}

// match
// -----
std::ostream &operator<<(std::ostream &os, const match &m) {
  os << "qry str idx: " << m.query_str_index
     << " txt str idx: " << m.text_str_index
     << " txt char idx: " << m.text_char_index
     << " match length: " << m.match_length << " beyond txt: " << m.beyond_txt
     << " str: " << m.str;
  return os;
}

// graph slice
// ------

std::ostream &operator<<(std::ostream &os, const graph_slice &s) {
  os << "Graph slice {"
     << "\ntxt start: " << s.txt_start << "\nqry start: " << s.qry_start
     << "\nlen: " << s.len << "\nstr: " << s.str << "\n}" << std::endl;
  return os;
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
                      std::pair<match_st::STvertex, std::string> *text,
                      std::vector<match> *candidate_matches,
                      n_core::Parameters const &parameters) {

  std::vector<n_core::extended_match> match_positions;

  for (int qry_str_idx = 0; qry_str_idx < queries.size(); qry_str_idx++) {
    std::string qry_str = queries[qry_str_idx];
    

    match_positions = match_st::FindEndIndexes(qry_str.c_str(), &text->first, text->second.c_str());
    
    for (auto match_pos : match_positions) {

      candidate_matches->push_back(
        n_junctions::match{.query_str_index = (int)qry_str_idx,
                         .text_str_index = match_pos.str_idx,
                         .text_char_index = match_pos.chr_idx ,
                         .match_length = match_pos.match_length,
                         .beyond_txt = match_pos.beyond_text,
                         .str = qry_str.substr(0, match_pos.match_length)});
    }
  }
}

} // namespace junctions
