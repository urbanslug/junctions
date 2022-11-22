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

// ---
// own
// ----

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
  size_t size;
  size_t length;
};

struct LinearizedEDS {
  std::string str;
  std::vector<std::vector<int>> prev_chars;
};

#endif
