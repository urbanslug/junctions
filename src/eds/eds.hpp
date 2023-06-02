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
struct slice_eds {
  std::size_t start;
  std::size_t length;
  bool eps_slice;

  slice_eds(std::size_t s, std::size_t l ): start (s), length (l), eps_slice(false) {
  }
  slice_eds(std::size_t s, std::size_t l, bool b): start(s), length(l), eps_slice(b) {
  }

  friend std::ostream &operator<<(std::ostream &os, slice_eds &sl);
  friend bool operator==(const slice_eds &lhs, const slice_eds &rhs);
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
  std::vector<DegenerateLetter> data; // rename to d_letters
  // std::vector<std::vector<span>> str_offsets;
  std::vector<std::vector<slice_eds>> slices;
  // std::unordered_set<size_t> stops;
  // std::unordered_set<size_t> starts;
  size_t size;
  size_t epsilons; // number of epsilons
  size_t str_count;        // number of strings
  size_t length;

public:
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

  std::vector<DegenerateLetter>& get_d_letters();

  // TODO: how can this be a const reference?
  DegenerateLetter& get_d_letter(std::size_t letter_idx);

  std::vector<std::vector<slice_eds>>& get_slices();
  std::vector<std::string> &get_strs(std::size_t letter_idx);
  std::vector<slice_eds>& get_slice(std::size_t letter_idx);

  bool is_letter_eps(std::size_t letter_idx) const;

  std::pair<std::size_t, std::size_t>
  get_letter_boundaries(std::size_t letter_idx);

  // setters and modifiers
  void append_d_letter(DegenerateLetter d);
  void append_slice(std::vector<slice_eds> s);
  void inc_size();
  void inc_str_count();
  void inc_epsilons();

  // IO
  void print_properties();
  friend std::ostream& operator<<(std::ostream &os, EDS& e);
};

class Parser {
public:
  static EDS from_eds_in_chunks(const std::string& fp);

  static EDS from_eds(const std::string& fp);
};
} // namespace eds
#endif