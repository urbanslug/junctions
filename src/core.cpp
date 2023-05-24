#include <ostream>
#include <tuple>

#include "core.hpp"

ll nwd(ll a, ll b) { return !b ? a : nwd(b, a % b); }
template <class T> inline T sqr(const T &a) { return a * a; }

VS parse(string s) {
  string a;
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

/**
 * concatenate a vector of strings with a given characters interspersing them
 *
 *
 * @param[in]  v vector of input strings to concatenate
 * @param[in]  c the character to have in between the strings
 * @param[out] s the resulting concatenated string
 */
void join(const vector<string> &v, char c, string &s) {
  s.clear();

  for (vector<string>::const_iterator p = v.begin(); p != v.end(); ++p) {
    s += *p;
    if (p != v.end() - 1)
      s += c;
  }
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

// query result
// ------------


std::ostream &operator<<(std::ostream &os, const junctions::query_result &r) {
  os << "match length: " << r.match_length << " beyond text: " << r.beyond_text;
  os << std::endl << "results: " << std::endl;
  for (auto res: r.results) {
    os << "char idx: " << res.char_index << " str idx: " << res.string_index << std::endl;
  }
  return os;
}

bool operator==(const junctions::query_result &lhs, const junctions::query_result &rhs) {

  std::vector<match_locus> rv = rhs.results;
  std::vector<match_locus> lv = lhs.results;

  std::sort(lv.begin(), lv.end());
  std::sort(rv.begin(), rv.end());

  return std::tie(lhs.beyond_text, lhs.match_length, lv) ==
         std::tie(rhs.beyond_text, rhs.match_length, rv);
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



// suffix
// ------
std::ostream &operator<<(std::ostream &os, const suffix &value) {
  os << "{ qry_ltr_idx: " << value.query_letter_idx
     << ", str_idx: " << value.str_idx
     << ", start_idx: " << value.start_idx
     << " }";
  return os;
}

bool operator<(const suffix &lhs, const suffix &rhs) {
  return std::tie(lhs.query_letter_idx, lhs.str_idx, lhs.start_idx) <
         std::tie(rhs.query_letter_idx, rhs.str_idx, rhs.start_idx);
}

bool operator==(const suffix &lhs, const suffix &rhs) {
  return std::tie(lhs.query_letter_idx, lhs.str_idx, lhs.start_idx) ==
         std::tie(rhs.query_letter_idx, rhs.str_idx, rhs.start_idx);
}


// cell
// ----
bool operator<(const cell &lhs, const cell &rhs) {
  // return lhs.i < rhs.i || (lhs.i == rhs.i && lhs.j < rhs.j);
  return std::tie(lhs.i, lhs.j) < std::tie(rhs.i, rhs.j);
}

std::ostream &operator<<(std::ostream &os, const cell &value) {
  os << "(" << " j: " << value.j << ", i: " << value.i << ")";
  return os;
}

bool operator==(const cell &lhs, const cell &rhs) {
  // return lhs.i < rhs.i || (lhs.i == rhs.i && lhs.j < rhs.j);
  return std::tie(lhs.i, lhs.j) == std::tie(rhs.i, rhs.j);
}

// match info
// ----------
std::ostream &operator<<(std::ostream &os, const match_info &value) {
  os << "{ str_idx: " << value.str_idx
     << " start_idx: " << value.start_idx
     << " }";
  return os;
}

bool operator==(const match_info &lhs, const match_info &rhs) {
  // return lhs.i < rhs.i || (lhs.i == rhs.i && lhs.j < rhs.j);
  return std::tie(lhs.str_idx, lhs.start_idx) == std::tie(rhs.str_idx, rhs.start_idx);
}

// match data
// ----------
std::ostream &operator<<(std::ostream &os, const match_data &value) {
  os << "{ text_str_idx: " << value.text_str_idx
     << " text_start_idx: " << value.text_start_idx
     << " query_str_idx: " << value.query_str_idx
     << " query_start_idx " << value.query_start_idx
     << " length: " << value.length <<
    " }";
  return os;
}

bool operator==(const match_data &lhs, const match_data &rhs) {
  // return lhs.i < rhs.i || (lhs.i == rhs.i && lhs.j < rhs.j);
  return std::tie(lhs.text_str_idx, lhs.text_start_idx, lhs.query_str_idx,
                  lhs.query_start_idx, lhs.length) ==
         std::tie(rhs.text_str_idx, rhs.text_start_idx, rhs.query_str_idx,
                  rhs.query_start_idx, rhs.length);
}


// spread
// ------
std::ostream &operator<<(std::ostream &os, const spread &value) {
  os << "{ start: " << value.start << " len: " << value.len << " }";
  return os;
}

// ed string
std::ostream &operator<<(std::ostream &os, const core::ed_string &value) {
  os << (value == core::ed_string::q ? "Q" : "W");
  return os;
}

// enums
// -----
/*
std::ostream &operator<<(std::ostream &os, const core::witness &value) {
  switch (value) {
  case core::witness::longest:
    os << "longest";
    break;
  case core::witness::shortest:
    os << "shortest";
    break;
  }
  return os;
}
*/
