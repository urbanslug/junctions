#include "./core.hpp"
#include <utility>
#include <vector>

// #include "../eds/eds.hpp"
namespace n_core {
Parameters::Parameters() {
}

task Parameters::get_task() const { return this->t; }
void Parameters::set_task(task tsk){ this->t = tsk; }
bool Parameters::is_task(task tsk) { return this->t == tsk; }

algorithm Parameters::get_algo() const { return this->algo; }
void Parameters::set_algo(algorithm a){ this->algo = a; }
bool Parameters::is_algo(algorithm a) { return this->algo == a; }

void Parameters::set_verbosity(unsigned char v) { this->v = v; };
void Parameters::set_witness(bool b) { this->w = b; };

bool Parameters::gen_dot() const { return this->output_dot; }
bool Parameters::compute_witness() const { return this->w; }
bool Parameters::multiset() const { return this->size_of_multiset; }
witness Parameters::get_witness_choice() const { return this->witness_choice; }
unsigned char Parameters::verbosity() const { return this->v; }

std::pair<file_format, std::string> Parameters::get_w_fp() const {
  return std::make_pair(this->w_format, this->w_file_path);
}

std::pair<file_format, std::string> Parameters::get_q_fp() const {
  return std::make_pair(this->q_format, this->q_file_path);
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
  return
    std::tie(lhs.beyond_text, lhs.match_length, lhs.str_idx, lhs.chr_idx) ==
    std::tie(rhs.beyond_text, rhs.match_length, rhs.str_idx, rhs.chr_idx);
}

std::ostream &operator<<(std::ostream &os, const extended_match &r) {
  os << "beyond text: " << r.beyond_text << " match length: " << r.match_length
     << " str idx: " << r.str_idx << " char idx: " << r.chr_idx;
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
  void perform_matching(eds::EDS &txt_eds, std::size_t txt_letter_idx,
                        std::pair<match_st::STvertex, std::string> *text,
                        std::vector<std::string> const &queries,
                        std::vector<EDSMatch>* candidate_matches) {

  std::vector<match_st::STQueryResult> match_positions;

  for (std::size_t qry_str_idx{0}; qry_str_idx < queries.size(); qry_str_idx++) {
    std::string qry_str = queries[qry_str_idx];

    match_positions = match_st::FindEndIndexes(qry_str.c_str(),
                                               &text->first,
                                               text->second.c_str());

    for (auto match_pos : match_positions) {

      eds::slice_eds local_txt_slice = txt_eds.get_str_slice_local(
          txt_letter_idx,
          match_pos.get_txt_str_idx());

      // TODO: ??
      //txt_eds.str_start_local(txt_letter_idx, match_pos.get_str_idx()); 

      // subtruct number of dollar signs which correspond to str idx
      // subtruct local slice start position

      match_pos.get_txt_char_idx_mut() -= (match_pos.get_txt_str_idx() + local_txt_slice.start);
      // match_pos.set_char_idx(c_idx);

      candidate_matches->push_back(
        EDSMatch(qry_str_idx,
                 qry_str.substr(0, match_pos.get_match_length()),
                 match_pos)
        );
    }
  }
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

void graph_slice::dbg_print(int indent_level = 0) {
  std::cerr << indent(indent_level) << "Graph slice {" << std::endl
            << indent(indent_level + 1) << "txt start " << this->txt_start << std::endl
            << indent(indent_level + 1) << "qry start " << this->qry_start << std::endl
            << indent(indent_level + 1) << "q_m(" << this->q_m.first << ", " << this->q_m.second << ")" << std::endl
            << indent(indent_level + 1) << "t_m(" << this->t_m.first << ", " << this->t_m.second << ")" << std::endl
            << indent(indent_level + 1) << "len " << this->len << std::endl
            << indent(indent_level + 1) << "str " << this->str << std::endl
            << indent(indent_level) << "}" << std::endl;
}


} // namespace junctions
