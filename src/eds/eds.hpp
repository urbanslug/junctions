#ifndef EDS_HPP
#define EDS_HPP
/**
 * A parser for ED Strings
 *
 */
#include <cstddef>
#include <ostream>
#include <vector>
#include <valarray>

namespace eds {

// an eps slice will have a length of zero
// TODO: make a class
struct slice_eds {
  std::size_t start;
  std::size_t length;
  bool eps_slice;

  slice_eds(std::size_t s, std::size_t l ): start (s), length (l), eps_slice(false) {
  }
  slice_eds(std::size_t s, std::size_t l, bool b): start(s), length(l), eps_slice(b) {
  }

  // std::size_t start();
  // std::size_t length();
  // std::size_t is_eps();

  friend std::ostream &operator<<(std::ostream &os, slice_eds &sl);
  friend bool operator==(const slice_eds &lhs, const slice_eds &rhs);
};

/**
 * A boundary is the index within N in which a degenerate letter is within.
 * a left boundary is the index of the first character of the first string in N
 * the right boundary is the index of the last char of the last string in N
 */
struct LetterBoundary {
  std::size_t l; // left
  std::size_t r; // right

  LetterBoundary() : l(0), r(0) {}

  LetterBoundary(std::size_t l, std::size_t r) : l(l), r(r) {}

  std::size_t left() const { return this->l; }
  std::size_t right() const { return this->r; }
};

class DegenerateLetter {
  std::vector<std::string> data;
  bool has_epsilon;

public:
  DegenerateLetter();

  std::vector<std::string>& get_strs();

  std::size_t size();
  std::size_t size_chars();
  void reset();
  bool is_empty();
  bool is_eps() const;
  void set_epsilon(bool b);
  // sets the epsilon to true
  void set_epsilon();
  void append_string(std::string& s);
};

class EDS {
  // rename to d_letters
  std::vector<DegenerateLetter> data;


  // slices run from 0 to size - 1 (or N)
  // the epsilon takes a slice of size 0 but increases the offset
  // of the next slice by 1
  std::vector<std::vector<slice_eds>> slices;

  // number of chars in the EDS
  // an epsilon doesn't add to N
  size_t size;
  size_t epsilons; // number of epsilons
  size_t str_count; // number of strings
  //size_t length; // TODO: remove. is never set

  // TODO: is there a better way to do this?
  // maybe a different object &inheritance?
public:
  std::string str;
  std::vector<std::vector<std::size_t>> prev_chars;


  EDS();

  // getters
  std::size_t get_size();
  std::size_t get_length() const;
  std::size_t get_eps_count();
  std::size_t get_str_count();
  std::size_t get_local_eps_idx(std::size_t letter_idx);
  std::size_t get_global_eps_idx(std::size_t letter_idx);
  std::size_t to_local_idx(std::size_t letter_idx, std::size_t global_idx);
  std::size_t to_global_idx(std::size_t letter_idx, std::size_t local_idx);

  std::size_t str_start_local(std::size_t letter_idx, std::size_t str_idx);
  // the slice of a str with a local start
  slice_eds get_str_slice_local(std::size_t letter_idx, std::size_t str_idx);

  std::vector<DegenerateLetter>& get_d_letters();

  // TODO: how can this be a const reference?
  DegenerateLetter& get_d_letter(std::size_t letter_idx);

  std::vector<std::vector<slice_eds>>& get_slices(); // rename to get_all_slices
  std::vector<std::string> &get_strs(std::size_t letter_idx);
  std::vector<slice_eds>& get_slice(std::size_t letter_idx); // rename to get_letter_slice

  std::vector<std::size_t> get_letter_ends_global(std::size_t letter_idx);

  bool is_letter_eps(std::size_t letter_idx) const;

  // the global start and end of the degenerate letter
  LetterBoundary get_letter_boundaries(std::size_t letter_idx);

  std::vector<std::size_t>& get_prev_char_idx(std::size_t char_idx);
  char get_char_at(std::size_t char_idx);

  // setters and modifiers
  void append_d_letter(DegenerateLetter d);
  void append_slice(std::vector<slice_eds> s);
  void inc_size();
  void inc_size_by(std::size_t n);
  void inc_str_count();
  void inc_epsilons();

  // IO
  void print_properties();
  void print_eds();
  friend std::ostream& operator<<(std::ostream &os, EDS& e);
  

  // populates str and prev_chars
  void linearize();
};


class Parser {
public:
  static EDS from_eds_in_chunks(const std::string& fp);

  static EDS from_eds(const std::string& fp);

  static EDS from_string(const std::string &raw_ed_string);

  static EDS from_msa(const std::string &fp);
};
} // namespace eds
#endif
