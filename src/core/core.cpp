#include <iostream>
#include <utility>
#include <vector>

#include "./core.hpp"
#include "constants.hpp"

namespace match_st {
  bool operator==(const STQueryResult& lhs, const STQueryResult& rhs) {

	std::size_t w = lhs.is_beyond_txt();
	std::size_t x = lhs.get_match_length();
	std::size_t y = lhs.get_char_idx();
	std::size_t z = lhs.get_txt_str_idx();


	std::size_t a = rhs.is_beyond_txt();
	std::size_t b = rhs.get_match_length();
	std::size_t c = rhs.get_char_idx();
	std::size_t d = rhs.get_txt_str_idx();
	
	return std::tie(w, x, y, z) == std::tie(a, b, c, d);
  }
}

namespace core {
AppConfig::AppConfig() {
  this->t = task::unset;
  this->output_dot = false;
  this->size_of_multiset = false;
  this->w = false;
  this->compute_match_stats_avg = false;
  this->compute_match_stats = false;
  this->compute_dist = false;
  this->compute_similarity = false;
  this->constraint = 0;
}

task AppConfig::get_task() const { return this->t; }
void AppConfig::set_task(task tsk){ this->t = tsk; }
bool AppConfig::is_task(task tsk) { return this->t == tsk; }

algorithm AppConfig::get_algo() const { return this->algo; }
void AppConfig::set_algo(algorithm a){ this->algo = a; }
bool AppConfig::is_algo(algorithm a) { return this->algo == a; }

void AppConfig::set_verbosity(unsigned char v) { this->v = v; };
void AppConfig::set_witness(bool b) { this->w = b; };

bool AppConfig::gen_dot() const { return this->output_dot; }
bool AppConfig::compute_witness() const { return this->w; }
bool AppConfig::multiset() const { return this->size_of_multiset; }
witness AppConfig::get_witness_choice() const { return this->witness_choice; }
unsigned char AppConfig::verbosity() const { return this->v; }

std::pair<file_format, std::string> AppConfig::get_w_fp() const {
  return std::make_pair(this->t1_format, this->t1_file_path);
}

std::pair<file_format, std::string> AppConfig::get_q_fp() const {
  return std::make_pair(this->t2_format, this->t2_file_path);
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
std::ostream &operator<<(std::ostream &os, const ed_string_e &value) {
  os << (value == ed_string_e::t1 ? core::constants::T_1 : core::constants::T_2);
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
                      std::vector<EDSMatch>* candidate_matches,
					  bool end_in_imp_imp) {

  std::vector<match_st::STQueryResult> match_positions;

  for (std::size_t qry_str_idx{0}; qry_str_idx < queries.size(); qry_str_idx++) {
    std::string qry_str = queries[qry_str_idx];

	//std::cout << "qry: " << qry_str << " txt: " << text->second << std::endl;
	
    match_positions =
	  match_st::FindEndIndexes(qry_str.c_str(),
							   &text->first,
							   text->second.c_str(),
							   end_in_imp_imp);


	
	
    for (match_st::STQueryResult match_pos : match_positions) {

	  /*
	  std::cout << "char idx: " << match_pos.get_char_idx()
				<< " txt idx: " << match_pos.get_txt_str_idx() 
				<< " match len: " << match_pos.get_match_length()
				<< std::endl;
	  */
	  
      eds::slice_eds local_txt_slice =
        txt_eds.get_str_slice_local(txt_letter_idx, match_pos.get_txt_str_idx());

      // subtruct number of dollar signs which correspond to str idx
      // subtruct local slice start position
      match_pos.get_txt_char_idx_mut() -=
        (match_pos.get_txt_str_idx() + local_txt_slice.start);


	  
      candidate_matches->push_back(
        EDSMatch(qry_str_idx,
                 qry_str.substr(0, match_pos.get_match_length()),
                 match_pos));
    }
  }
}
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

  for (std::vector<std::string>::const_iterator p = v.begin(); p != v.end(); ++p) {
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


} // namespace core

