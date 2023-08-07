#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <valarray>
#include <vector>
#include <iomanip>
#include <immintrin.h> // Include the AVX header
#include <set>
#if __cplusplus >= 202002L
#include <format>
#endif

#include "eds.hpp"
#include "constants.hpp"


namespace eds {
const std::streamsize CHUNK_SIZE = 4096;
const std::size_t REGISTER_WIDTH = 8;

inline bool isDNA(char ch) {
  return (ch == 'A' || ch == 'T' || ch == 'C' || ch == 'G' || ch == 'N');
}

inline bool is_open_bracket(char ch) { return ch == '{'; }
inline bool is_close_bracket(char ch) { return ch == '}'; }
inline bool is_comma(char ch) { return ch == ','; }
inline  bool is_separator(char ch) {
  return (is_open_bracket(ch) || is_close_bracket(ch) || is_comma(ch));
}

#define FILE_ERROR(name)                                                       \
  {                                                                            \
	std::string e = "Error, Failed to open the file" + name; \
	throw std::invalid_argument(e);                                              \
  }


int computeMultiple(int number) {
  if (number % REGISTER_WIDTH == 0) {
	return number;
  } else {
	return number + (REGISTER_WIDTH - number % REGISTER_WIDTH);
  }
}

void sumColumnsAVX(
  const float* vec1,
  const float* vec2,
  float* result,
  std::size_t numCols
) {
  // The following code assumes that numRows and numCols are multiples of 8
  for (size_t col = 0; col < numCols; col += 8) {
	// Initialize an AVX register with zeros
	__m256 sum = _mm256_setzero_ps();

	// Load 8 elements from each vector into AVX registers
	__m256 v1 = _mm256_loadu_ps(&vec1[col]);
	__m256 v2 = _mm256_loadu_ps(&vec2[col]);

	// Add the elements from both vectors
	sum = _mm256_add_ps(v1, v2);

	// Store the result of the sum back to memory
	_mm256_storeu_ps(&result[col], sum);
  }
}

void sumColumnsAVX_a(
  float* vec1,
  const float* vec2,
  std::size_t numCols
) {
  // The following code assumes that numRows and numCols are multiples of 8
  for (size_t col = 0; col < numCols; col += 8) {
	// Initialize an AVX register with zeros
	__m256 sum = _mm256_setzero_ps();

	// Load 8 elements from each vector into AVX registers
	__m256 v1 = _mm256_loadu_ps(&vec1[col]);
	__m256 v2 = _mm256_loadu_ps(&vec2[col]);

	// Add the elements from both vectors
	sum = _mm256_add_ps(v1, v2);

	// Store the result of the sum back to memory
	_mm256_storeu_ps(&vec1[col], sum);
  }
}

std::valarray<std::size_t> xor_vectors(
  std::valarray<std::size_t> const& a,
  std::valarray<std::size_t> const& b
) {
  std::valarray<std::size_t> c = (a ^ b);
  return c;
}

std::valarray<std::size_t>
compute_similar_regions(std::vector<std::string> const& raw_strings) {
  std::size_t cols{raw_strings.front().length()};
  std::size_t rows{raw_strings.size()};

  std::valarray<std::size_t> a(cols);
  std::valarray<std::size_t> b(cols);

  //std::cout << "cols: " << cols << std::endl;

  std::vector<std::valarray<std::size_t>> xor_vecs{};

  for (std::size_t row{}; row < rows-1; ++row) {

	//std::cout << raw_strings[row] << std::endl;

	//std::cout << raw_strings[row+1] << std::endl;

	for (std::size_t col{}; col < cols; ++col) {

	  char c = raw_strings[row][col];
	  char _c = raw_strings[row+1][col];

	  //std::cout << "c: " << c << " _c: " << _c << std::endl;

	  a[col] = c;
	  b[col] = _c;
	}

	//std::valarray<std::size_t> res = xor_vectors(a, b);
	xor_vecs.push_back(xor_vectors(a, b));

  }

  //for (auto i : xor_vecs) {
  //for (int j = 0; j < cols; ++j) {
  //   std::cout  << std::left << std::setw(4) << i[j] << " ";
  //}
  //std::cout << "\n";
  //}

  //std::cout << "cols: " << cols << std::endl;

  std::size_t x = computeMultiple(cols);

  float _result[x];

  for (int i = 0; i < cols; ++i) {
	_result[i] = static_cast<float>(xor_vecs[0][i]);
  }
  if (cols < x) {
	for (int i = cols; i < x; ++i) {
	  _result[i] = 0;
	}
  }

  // TODO: is this always rows-1?
  std::size_t xor_rows = xor_vecs.size();




  //return;

  float vec1[x];
  for (std::size_t row {1}; row < xor_rows; ++row) {
	// populate vectors vec1 and vec2

	// std::cout << "row: " << row << std::endl;

	for (std::size_t i{}; i < cols; ++i) {
	  vec1[i] = static_cast<float>(xor_vecs[row][i]);
	}

	if (cols < x) {
	  for (std::size_t i = cols; i < x; ++i) { vec1[i] = 0; }
	}

	sumColumnsAVX_a(_result, vec1, x);
  }

  // print the _result vector
  //for (int i = 0; i < cols; ++i) {
  //std::cout << std::left << std::setw(4) << _result[i] << " ";
  //}

  // convert my float array to a std::size_t array
  // cols or x ?
  std::valarray<std::size_t> result(cols);
  for (int i = 0; i < cols; ++i) {
	result[i] = static_cast<std::size_t>(_result[i]);
  }
  // std::cout << std::endl;

  return result;
}

struct Foo {
  bool similar;
  std::size_t start;
  std::size_t length;
};

std::vector<Foo>
compute_region_vector(std::vector<std::string> const& raw_strings) {
  std::valarray<std::size_t> r = compute_similar_regions(raw_strings);

  std::size_t start{};
  std::size_t length{};
  std::size_t prev{r[0]};

  std::vector<Foo> foo{};

  std::vector<std::pair<std::size_t, std::size_t>> similar{};
	std::vector<std::pair<std::size_t, std::size_t>> different{};

  for (std::size_t i{}; i < r.size(); ++i) {
	if (r[i]  == 0 && prev == 0 ) {
	  // continue with zeros
	  ++length;
	}
	else if (prev == 0 && r[i] != 0) {
	  // start of non zeros
	  //std::cout << "start: " << start << " length: " << length << std::endl;
	  //similar.push_back(std::make_pair(start, length));
	  foo.push_back({true, start, length});
	  start = i;
	  length = 1;
	}
	else if (prev != 0 && r[i] != 0) {
	  // continue with non-zeros
	  ++length;
	}
	else if (prev != 0 && r[i] == 0) {
	  // end of non-zeros & start of zeros
	  //std::cout << "start: " << start << " length: " << length << std::endl;
	  //different.push_back(std::make_pair(start, length));
	  foo.push_back({false, start, length});
	  start = i;
	  length = 1;
	}
	else {
	  std::cout << "unexpected state" << std::endl;
	  //std::cout << "1start: " << start << " length: " << length << std::endl;
	  start = i;
	  length = 0;
	}
	prev = r[i];
  }

  // the last one
  if (prev == 0) {
	foo.push_back({true, start, length});
  }
  else {
	foo.push_back({false, start, length});
  }

  return foo;
}

void parse_msa_data(std::vector<std::string> const& raw_strings, EDS* e) {
  std::vector<Foo> foo = compute_region_vector(raw_strings);
  DegenerateLetter d = DegenerateLetter();
  std::vector<eds::slice_eds> d_slice{};
  std::set<std::string, std::greater<std::string>> d_set{};

  std::string current_string{};
  std::size_t rows{raw_strings.size()};
  std::size_t counter{};

  for (auto i : foo) {

	//std::cout << "start: " << i.start << " length: " << i.length << std::endl;
	
	if (i.similar) {
	  current_string = raw_strings[0].substr(i.start, i.length);

	  std::string ss{};

	  for (char c: current_string) {
		  if (c != '-') { ss += c; }
	  }

	  if (ss.empty()) { continue; }
	  
	  d.append_string(ss);

	  d_slice.push_back(eds::slice_eds(counter, ss.length()));
	  counter += ss.length();

	  e->inc_size_by(ss.length());
	  e->inc_str_count();
	}
	else {
	  for (std::size_t row{}; row < rows; ++row) {
		current_string = raw_strings[row].substr(i.start, i.length);
		d_set.insert(current_string);
	  }

	  for (std::string s: d_set) {
		std::string ss{};
		for (char c: s) {
		  if (c != '-') { ss += c; }
		}

		if (ss.empty()) {
		  d.set_epsilon(true);

		  d_slice.push_back(eds::slice_eds(counter, 0, true));

		  e->inc_epsilons();
		  //continue;
		  ++counter;
		}
		else {
		  e->inc_size_by(ss.length());
		  d.append_string(ss);
		  d_slice.push_back(eds::slice_eds(counter, ss.length()));
		  counter = counter + ss.length();
		}

	  e->inc_str_count();

		//d_slice.push_back({counter, ss.length()});

	  }

	}

	e->append_d_letter(d);
	e->append_slice(d_slice);

	d.reset();
	d_set.clear();
	d_slice.clear();
  }


  // std::cerr << *e << std::endl;
  // e->print_properties();

  // std::cout << std::endl << std::endl;
  
  //e->print_eds();

  //std::cout << std::endl;
  return;

  for (auto s : e->get_slices()) {
	for (auto sl: s) {
	  std::cout << "start: " << sl.start << " length: " << sl.length << " epsilon: " << sl.eps_slice << std::endl;
	}
	std::cout << std::endl;
  }

}


/**
 * parse the entire string
 * works with input from read_as_txt
 */
 // TODO: use string_view
void parse_data(const std::string& raw_string, EDS* e) {
  DegenerateLetter d = DegenerateLetter();
  std::string current_string{};
  // For use with errors. A counter for the current character
  std::size_t counter{};
  // points to the start of the current string we are processing in N
  // epsilon takes up the last position in a letter
  // even though it is not considered in size calculation
  // the start of the current string in N used to compute slices
  std::size_t needle{};
  std::vector<eds::slice_eds> d_slice{};
  char c{};

  auto reset_vars = [&]() {
	d.reset();
	d_slice.clear();
	current_string.clear();
  };

  auto update_slice = [&]() {
	if (!current_string.empty()){
	  d_slice.push_back(eds::slice_eds(needle, current_string.length()));
	  needle += current_string.length();
	}

	if (d.is_eps() && is_close_bracket(c)) {
	  d_slice.push_back(eds::slice_eds(needle, 0, true));
	  ++needle;
	}
  };


  for (auto it = raw_string.begin(); it != raw_string.end(); it++) {
	c = *it;

	if (isDNA(c)) {
	  e->inc_size();
	  current_string += c;
	}
	else if (is_open_bracket(c)) {
	  // TODO: merge the two following
	  // d should be empty
	  if (!current_string.empty()){
		d.append_string(current_string);

		update_slice();
		e->append_slice(d_slice);

		e->append_d_letter(d);
		e->inc_str_count();
	  }
	  reset_vars();
	}
	else if (is_close_bracket(c)) {
	  if (current_string.empty() && !d.is_eps() ) {
		// we found an epsilon at the end of the letter
		d.set_epsilon();
		e->inc_epsilons();
	  }
	  if (!d.is_empty() || !current_string.empty()) {
		// we have finished parsing a d letter
		if (!current_string.empty()) { d.append_string(current_string);}

		update_slice();
		e->append_slice(d_slice);

		e->append_d_letter(d);
		e->inc_str_count();

		//if (d.is_eps()) { ++needle; }
	  }
	  reset_vars();
	}
	else if (is_comma(c) && !current_string.empty()) {
	  // save the current string to d
	  d.append_string(current_string);
	  e->inc_str_count();

	  update_slice();

	  current_string.clear();
	}
	else if (is_comma(c) && current_string.empty() && !d.is_eps()){
	  // we found a valid epsilon
	  d.set_epsilon();
	  e->inc_epsilons();

	  // its slice information is set at the end of the letter
	}
	else if (isspace(c) || is_comma(c)) {

	  // ignore whitespace
		// second comma is duplicate eps
	  continue;
	}
	else {
	  std::string e = "Error, parsing failed found " + std::to_string(c) + "at position " + std::to_string(counter) + "zero indexed.";
	  throw std::invalid_argument(e);
	}

	if (d.is_empty() && it == raw_string.end() - 1) {
	  // we have finished parsing the input so save last string
	  if (!current_string.empty()) {

		d.append_string(current_string);

		update_slice();
		e->append_slice(d_slice);

		e->append_d_letter(d);
		e->inc_str_count();

		// if (d.is_eps()) { ++needle; }
	  }
	  reset_vars();
	}

	++counter;
  }
}

/**
 * parse the chunk we just read
 * works with input from read_chunks
 */
void parse_data(char (&buf)[CHUNK_SIZE], std::streamsize extracted ) {
  char c {};
  for (int i{}; i < extracted; i++) {
	c = buf[i];
	if (isDNA(c)) {
	  // std::cout << buf[i];
	}
	else if (is_separator(c)) {
	  //
	}
	else if (isspace(c)) {
	  continue;
	}
	else  {
	  // TODO: specify position
	  // TODO: check if can use format
	  std::cerr << "parsing failed found " << c << "at position " << std::endl;
	  exit(1);
	}
  }
}

/**
 * read a file in chunks of chunk size
 * keeps memory low
 * for large files
 */
void read_chunks(const std::string& fp) {
  std::ifstream f{fp};

  if (!f) { FILE_ERROR(fp); }

  char buf[CHUNK_SIZE];
  std::string ss;
  std::streamsize extracted;

  while (!f.eof()) {
	f.read(buf, CHUNK_SIZE);
	extracted = f.gcount();
	parse_data(buf, extracted);
  }
}

/**
 * read the entire file into a string
 * should be faster for small inputs to read the entire file into a string and process
 * it at once
 */
void read_as_txt_noformat(const std::string& fp, std::string& raw_string) {
  std::ifstream f{fp};
  std::stringstream ss;

  if (!f) { FILE_ERROR(fp); }

  while (ss << f.rdbuf()) {
	raw_string.append(ss.str());
  }
}

/**
 * read the entire file into a string
 * should be faster for small inputs to read the entire file into a string and
 * process it at once
 * will perform whitespace normalization/formatting
 */
void read_as_format_txt(const std::string& fp, std::string *raw_string) {
  std::ifstream f{fp};
  std::string temp;

  if (!f) { FILE_ERROR(fp); }

  while (f >> temp) {
	raw_string->append(temp);
  }
}

void read_msa_as_format_txt(const std::string& fp, std::vector<std::string>* raw_strings) {
  std::ifstream f{fp};
  std::string temp;

  if (!f) { FILE_ERROR(fp); }

  auto starts_with = [&]() -> bool {
	return temp[0] == '>';
  };

  while (std::getline(f, temp)) {
	if (starts_with()) {
	  raw_strings->push_back(std::string());
	} else {
	  raw_strings->back().append(temp);
	}
  }

  // TODO: remove
  /*
  while (f >> temp) {
	if (starts_with()) {
	  raw_strings->push_back(std::string());
	} else {
	  raw_strings->back().append(temp);
	}
  }*/
}

/**
 * Get the size of a file
 */
std::size_t get_file_size(const std::string& fp) {
  std::streampos begin, end;
  std::ifstream f (fp);

  if (!f) { FILE_ERROR(fp); }

  begin = f.tellg();
  f.seekg (0, std::ios::end);
  end = f.tellg();
  f.close();

  return end-begin;
}
// -------
// Structs
// -------

// slice_eds
// ---------
std::ostream &operator<<(std::ostream &os, const eds::slice_eds &sl) {
  std::string s;
#if __cplusplus >= 202002L
  s = std::format("start: {}, length: {}, is eps: {}", sl.start, sl.length,
			  sl.eps_slice);
#else
  s = "start: " + std::to_string(sl.start) +
	  " length: " + std::to_string(sl.length) +
	  " is eps: " + std::to_string(sl.eps_slice);
#endif
  os << s;
  return os;
}

bool operator==(const eds::slice_eds &lhs, const eds::slice_eds &rhs) {
  return std::tie(lhs.start, lhs.length, lhs.eps_slice) ==
		 std::tie(rhs.start, rhs.length, rhs.eps_slice);
}

// -------
// Classes
// -------

// Degenerate letter
// -----------------
DegenerateLetter::DegenerateLetter() {
  data = std::vector<std::string>{};
  has_epsilon = false;
}

void DegenerateLetter::reset() {
  this->data.clear();
  this->has_epsilon = false;
};

void DegenerateLetter::append_string(std::string& s){ this->data.push_back(s); }

void DegenerateLetter::set_epsilon(bool b) { this->has_epsilon = b; }
void DegenerateLetter::set_epsilon() { this->has_epsilon = true; }

// number of strings in the d_letter
std::size_t DegenerateLetter::size() {
  return this->is_eps() ? this->data.size() + 1 : this->data.size();
}

// number of strings in the d_letter
std::size_t DegenerateLetter::size_chars() {
  int count{};
  for(auto s: this->data) {
	count += s.size();
  }
  return count;
}

bool DegenerateLetter::is_empty() { return this->data.empty(); }
bool DegenerateLetter::is_eps() const { return this->has_epsilon; }

std::vector<std::string>& DegenerateLetter::get_strs() { return this->data; }

// EDS
// ----

EDS::EDS() {
  data = std::vector<DegenerateLetter>{};
  slices = std::vector<std::vector<eds::slice_eds>>{};
  size = 0;
  epsilons = 0;
  str_count = 0;
  //length = 0;

  str = std::string{};
  prev_chars = std::vector<std::vector<std::size_t>>{};
}

void EDS::linearize() {
  std::size_t expected_size = this->get_size() + this->get_eps_count();
  this->str.reserve(expected_size);
  this->prev_chars.reserve(expected_size);


  // current and previous eds slice
  std::vector<slice_eds> c_sl{};
  std::vector<slice_eds> p_sl{};
  bool p_eps{false};

  for (std::size_t i{}; i < this->get_length(); i++){
	// populate strings
	for (auto s :this->get_strs(i)) { this->str.append(s); }
	if (this->is_letter_eps(i)) { this->str += '*'; }

	// populate previous char indexes
	eds::LetterBoundary bounds = this->get_letter_boundaries(i);
	c_sl = this->get_slice(i);

	std::size_t k = bounds.left();
	for (; k <= bounds.right(); k++) {
	  this->prev_chars.push_back(std::vector<std::size_t>{});
	  std::vector<slice_eds>::iterator it;
	  it = std::find_if(c_sl.begin(), c_sl.end(),
					 [k](slice_eds s) {return s.start == k;} );

	  if (it == c_sl.end()) {
		prev_chars.at(k).push_back(k - 1);
	  } else {
		for (slice_eds p_s: p_sl) {
		  this->prev_chars.at(k).push_back(p_s.start + p_s.length - 1);
		}

		if (p_eps) {
		  this->prev_chars.at(k).push_back(this->get_global_eps_idx(i-1));
		}
	  }
	}

	p_eps = this->is_letter_eps(i);
	p_sl = c_sl;
  }
}


// getters
std::vector<std::size_t>& EDS::get_prev_char_idx(std::size_t char_idx) {
  return this->prev_chars.at(char_idx);
}

char EDS::get_char_at(std::size_t char_idx) {
  return this->str.at(char_idx);
}


std::size_t EDS::get_size() { return this->size; }
std::size_t EDS::get_length() const { return this->data.size(); }
std::size_t EDS::get_eps_count() { return this->epsilons; }
std::size_t EDS::get_str_count() { return this->str_count; }
std::vector<DegenerateLetter> &EDS::get_d_letters() { return this->data; }
std::vector<std::vector<eds::slice_eds>> &EDS::get_slices() {
  return this->slices;
}

DegenerateLetter& EDS::get_d_letter(std::size_t letter_idx) {
  return this->data.at(letter_idx);
}

std::vector<slice_eds>& EDS::get_slice(std::size_t letter_idx){
  return this->slices.at(letter_idx);
}

std::vector<std::size_t> EDS::get_letter_ends_global(std::size_t letter_idx) {
  std::vector<slice_eds>& sls = this->get_slice(letter_idx);
  bool e = this->is_letter_eps(letter_idx);
  std::vector<std::size_t> v;
  // TODO use a better iterator some kind of accumulator

  for (auto it = sls.begin(); it != sls.end(); it++) {
	if (e && it == sls.end() - 1) {
	  // we are at an epsilon slice
	  v.push_back(it->start + it->length);
	}  else {
	  // TODO: take care of 0 + 0 - 1
	  v.push_back(it->start + it->length - 1);
	}
  }
  return v;
}


bool EDS::is_letter_eps(std::size_t letter_idx) const {
	return this->data.at(letter_idx).is_eps();
}

std::size_t EDS::to_local_idx(std::size_t letter_idx, std::size_t global_idx){
  return global_idx - this->get_slices().at(letter_idx).front().start;
}
std::size_t EDS::to_global_idx(std::size_t letter_idx, std::size_t local_idx) {
  return local_idx + this->get_slices().at(letter_idx).front().start;
}

std::size_t EDS::str_start_local(std::size_t letter_idx, std::size_t str_idx) {
  std::vector<slice_eds> &sl = this->get_slice(letter_idx);
  return sl.at(str_idx).start - sl.front().start;
}

eds::slice_eds EDS::get_str_slice_local(std::size_t letter_idx, std::size_t str_idx) {
  std::vector<slice_eds> &sl = this->get_slice(letter_idx);
  eds::slice_eds local_sl = sl.at(str_idx);
  local_sl.start = sl.at(str_idx).start - sl.front().start;
  return local_sl;
}

std::size_t EDS::get_local_eps_idx(std::size_t letter_idx) {
  size_t gl_idx = this->get_global_eps_idx(letter_idx);
  return this->to_local_idx(letter_idx, gl_idx);
}

std::size_t EDS::get_global_eps_idx(std::size_t letter_idx) {
  if (!this->get_d_letter(letter_idx).is_eps()) {
#if __cplusplus >= 202002L
	throw std::domain_error(
		std::format("letter {} does not have an eps", letter_idx));
#else
	std::string e =
		"letter " + std::to_string(letter_idx) + " does not have an eps";
	throw std::domain_error(e);
#endif

  }
  return this->get_slice(letter_idx).back().start;
}



void EDS::inc_size() { ++this->size; }
void EDS::inc_size_by(std::size_t n) { this->size += n; }

void EDS::inc_epsilons() { ++this->epsilons; }
void EDS::inc_str_count() { ++this->str_count; }

void EDS::append_d_letter(DegenerateLetter d) { this->data.push_back(d); }
void EDS::append_slice(std::vector<eds::slice_eds> s) { this->slices.push_back(s); }

std::vector<std::string>& EDS::get_strs(std::size_t letter_idx) {
  return this->get_d_letter(letter_idx).get_strs();
  ;
}

eds::LetterBoundary EDS::get_letter_boundaries(std::size_t letter_idx){

  std::vector<eds::slice_eds> s = this->get_slice(letter_idx);
  std::size_t left, right;
  left = s.front().start;
  right = s.back().start + s.back().length;

  if (this->is_letter_eps(letter_idx)) {
	return eds::LetterBoundary(left, right);
  } else {
	// TODO: right should never be zero if so throw an exception
	return eds::LetterBoundary(left, right-1);
  }
};

// TODO: clean up and move magic numbers to constants
  void EDS::print_eds() {
  std::size_t line_len{};
  std::size_t max_line_len = 80;

  for (auto d : this->get_d_letters()) {
	if (line_len > 0 && d.size_chars() + line_len > max_line_len) {
	  std::cout << "\n";
	  line_len = 0;
	}
	if (d.size() > 1) {
	  std::cout << "{";
	  line_len++;
	}
	for (auto s = d.get_strs().begin(); s != d.get_strs().end(); s++) {
	  if (line_len >0 && line_len + s->length() > max_line_len && d.size() == 1) {
		std::cout << "\n";
		line_len = 0;
	  } else {
		line_len += s->length();
	  }
	  std::cout << *s;
	  if ( d.size() > 1 && s != d.get_strs().end() - 1) { std::cout << ","; }
	  if (d.size() > 1 && s == d.get_strs().end() - 1 && d.is_eps() ) {
		std::cout << "," ;
	  }
	}
	if (d.size() > 1) {
	  std::cout << "}";
	  line_len++;
	}

	if (line_len > 0 && line_len > max_line_len) {
	  std::cout << "\n";
	  line_len = 0;
	}
  }
  //os << "\n";
  //return os;
}
  
std::ostream& operator<<(std::ostream &os, EDS& e) {
  std::size_t line_len{};
  std::size_t max_line_len = 80;

  for (auto d : e.get_d_letters()) {
	if (line_len > 0 && d.size_chars() + line_len > max_line_len) {
	  os << "\n";
	  line_len = 0;
	}
	if (d.size() > 1) {
	  os << "{";
	  line_len++;
	}
	for (auto s = d.get_strs().begin(); s != d.get_strs().end(); s++) {
	  if (line_len >0 && line_len + s->length() > max_line_len && d.size() == 1) {
		os << "\n";
		line_len = 0;
	  } else {
		line_len += s->length();
	  }
	  os << *s;
	  if ( d.size() > 1 && s != d.get_strs().end() - 1) { os << ","; }
	  if (d.size() > 1 && s == d.get_strs().end() - 1 && d.is_eps() ) {
		os << "," << constants::unicode_eps;
	  }
	}
	if (d.size() > 1) {
	  os << "}";
	  line_len++;
	}

	if (line_len > 0 && line_len > max_line_len) {
	  os << "\n";
	  line_len = 0;
	}
  }
  //os << "\n";
  return os;
}

void EDS::print_properties() {
  std::cerr << "size (N): " << this->get_size()
			<< " length (n): " << this->get_length()
			<< " str count (m): " << this->get_str_count()
			<< " number of epsilons: " << this->get_eps_count()
			<< "\n";
}



// Parser
// ------


EDS Parser::from_eds(const std::string &fp) {
  EDS n;
  std::string raw_string;
  size_t file_size = get_file_size(fp);

  raw_string.reserve(file_size);
  read_as_format_txt(fp, &raw_string);
  raw_string.shrink_to_fit();

  parse_data(raw_string, &n);

  return n;
}

EDS Parser::from_string(const std::string &raw_string) {
  EDS n;
  parse_data(raw_string, &n);
  return n;
}

EDS Parser::from_msa(const std::string &fp) {
  EDS n;
  std::vector<std::string> raw_strings;
  std::string raw_string;
  size_t file_size = get_file_size(fp);

  raw_string.reserve(file_size);
  read_msa_as_format_txt(fp, &raw_strings);
  raw_string.shrink_to_fit();

  parse_msa_data(raw_strings, &n);

  //exit(0);

  //parse_data(raw_string, &n);

  return n;
}

// Not implemented
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
EDS Parser::from_eds_in_chunks(const std::string &fp) {
  EDS n;
  return n;
}
#pragma GCC diagnostic pop

} // namespace eds
