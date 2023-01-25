#include "./core.hpp"
#include <ostream>
#include <tuple>

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
