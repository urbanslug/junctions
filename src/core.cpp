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

// match info
// ----------
std::ostream &operator<<(std::ostream &os, const match_info &value) {
  os << "{ str_idx: " << value.str_idx
     << " start_idx: " << value.start_idx
     << " }";
  return os;
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
