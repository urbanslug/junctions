#ifndef CORE_H
#define CORE_H

#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <set>
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>
#include <iterator>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <vector>
using namespace std;

/*
  Suffix tree
  ----
*/


#define FOR(I, A, B) for (int I = (A); I <= (B); I++)
#define FORD(I, A, B) for (int I = (A); I >= (B); I--)
#define REP(I, N) for (int I = 0; I < (N); I++)
#define ALL(X) (X).begin(), (X).end()
#define PB push_back
#define MP make_pair
#define FI first
#define SE second
#define INFTY 100000000
#define VAR(V, init) __typeof(init) V = (init)
#define FORE(I, C) for (VAR(I, (C).begin()); I != (C).end(); I++)
#define SIZE(x) ((int)(x).size())

typedef vector<int> VI;
typedef pair<int, int> PII;
typedef long long ll;
typedef vector<string> VS;

ll nwd(ll a, ll b);

VS parse(string s);

int toi(char ch);

int chg(char ch);

int los(int m);

// ---
// own
// ----

// C++ timer
typedef std::chrono::high_resolution_clock Time;

const uint8_t DEBUG_LEVEL = 0; // TODO: replace with verbosity in params

typedef std::vector<std::vector<bool>> matrix; // TODO: remove
typedef std::vector<std::vector<bool>> boolean_matrix;
typedef std::vector<std::vector<int>> int_matrix;
typedef std::vector<std::vector<std::string>> string_matrix;
typedef std::vector<std::size_t> size_t_vec;
typedef std::vector<std::string> string_vec;
typedef std::vector<int> int_vec;

struct degenerate_letter {
  std::vector<std::string> data;
  bool has_epsilon;
};

struct span {
  size_t start;
  size_t stop;
};

struct EDS {
  std::vector<degenerate_letter> data;
  std::vector<std::vector<span>> str_offsets;
  std::set<size_t> stops;
  std::set<size_t> starts;
  size_t size;
  size_t length;
};

struct LinearizedEDS {
  std::string str;
  std::vector<std::vector<int>> prev_chars;
};


struct cell {
  int j;
  int i;
};

bool operator<(const cell &lhs, const cell &rhs);

std::ostream &operator<<(std::ostream &os, const cell &value);

struct suffix {
  int query_letter_idx;
  int str_idx;
  int start_idx;

  std::string to_string() const {
    return "{ qry_ltr_idx " + std::to_string(query_letter_idx) +
      ", str_idx " + std::to_string(str_idx) +
      ", start_idx " + std::to_string(start_idx) +
      " }";
  }
};


std::ostream &operator<<(std::ostream &os, const suffix &value);

bool operator<(const suffix &lhs, const suffix &rhs);

struct spread {
  int start;
  int len;
};

std::ostream &operator<<(std::ostream &os, const spread &value);

struct match_info {
  int str_idx;
  int start_idx;
};

std::ostream &operator<<(std::ostream &os, const match_info &value);

// ----
// CLI
// ----

namespace core {
enum file_format {
  msa, // msa in PIR format
  eds, // eds file
  unknown
};

enum ed_string { w, q };

std::ostream &operator<<(std::ostream &os, const core::ed_string &value);

enum algorithm { naive, improved, both };

/**
 * @brief   parameters for cli args
 */
struct Parameters {
  algorithm algo;
  int verbosity;
  std::string w_file_path; // reference sequence(s)
  file_format w_format;
  std::string q_file_path; // query sequence(s)
  file_format q_format;
};
}


#endif
