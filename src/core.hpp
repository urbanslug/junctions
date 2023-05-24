#ifndef CORE_HPP
#define CORE_HPP


#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <set>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

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
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>



using namespace std;
/*
 * Own
 */

namespace junctions {

const std::string unicode_eps = "\u03B5";
const std::string unicode_sub_1 = "\u2081";
const std::string unicode_sub_2 = "\u2082";

const std::string T_1 = "T\u2081";
const std::string T_2 = "T\u2082";
const std::string N_1 = "N\u2081";
const std::string N_2 = "N\u2082";
const std::string q_0 = "q\u2080";
const std::string q_a = "q\u2090";

std::string indent(int level);

void join(const vector<string> &v, char c, string &s);

/**
 * ->
 */
bool logical_implication(bool p, bool q);

/**
 * <->
 */
bool double_implication(bool a, bool b);

/**
 *
 * Is a match explicit or implicit
 *
 */
enum match_type { exp, imp };

// typedef std::pair<match_pe, match_type> vertex_type;

struct graph_slice {
  int txt_start;
  int qry_start;

  std::pair<match_type, match_type> q_m;
  std::pair<match_type, match_type> t_m;

  int len;
  std::string str;

  void dbg_print(int indent_level = 0) {
    std::cerr << indent(indent_level) << "Graph slice {" << std::endl
              << indent(indent_level + 1) << "txt start " << this->txt_start << std::endl
              << indent(indent_level + 1) << "qry start " << this->qry_start << std::endl
              << indent(indent_level + 1) << "q_m(" << this->q_m.first << ", " << this->q_m.second << ")" << std::endl
              << indent(indent_level + 1) << "t_m(" << this->t_m.first << ", " << this->t_m.second << ")" << std::endl
              << indent(indent_level + 1) << "len " << this->len << std::endl
              << indent(indent_level + 1) << "str " << this->str << std::endl
              << indent(indent_level) << "}" << std::endl;
  }
};

std::ostream &operator<<(std::ostream &os, const junctions::graph_slice &s);

// or match positon?
struct match_locus {
  int string_index; // rename to str_index
  int char_index; // is this always zero?

  // candidate active suffix match
  bool candidate_asm() {
    return !(this->char_index == 0);
  }
};

bool operator<(const match_locus &lhs, const match_locus &rhs);

typedef match_locus locus;

struct query_result {
  bool beyond_text; // there was a match but the query was longer than the text
  int match_length;
  std::vector<match_locus> results;

  query_result() {
    this->results = std::vector<match_locus>{};
    this->match_length = -1;
    this->beyond_text=false;
  }

  bool is_valid() { return this->match_length > 0 && !this->results.empty(); }

  bool is_hit() { return this->match_length > 0 && !this->results.empty(); }

  bool is_miss() { return !this->is_hit(); }
};

std::ostream &operator<<(std::ostream &os, const junctions::query_result &r);

bool operator==(const junctions::query_result &lhs, const junctions::query_result &rhs);

// generalization of query result and match locus above
// replace query_result an match_locus with this
struct extended_match {
  bool beyond_text; // there was a match but the query was longer than the text
  int match_length; // the length of the match
  int str_idx; // the index of the text string

  // the match position in the conactenated string ignoring $
  // TODO: make this char idx in the actual txt string
  int chr_idx;

  /*
  extended_match() {
    this->beyond_text = false;
    this->match_length = -1;
    this->str_idx = -1;
    this->chr_idx = -1;
  }*/

};

std::ostream &operator<<(std::ostream &os, const junctions::extended_match &r);

bool operator==(const junctions::extended_match &lhs, const junctions::extended_match &rhs);

struct match {
  int query_str_index;
  // no need for query_char_idx bexause it's always 0

  int text_str_index;
  int text_char_index;

  int match_length;
  bool beyond_txt;

  std::string str;
  // std::string t_str;

  // bool is_active_suffix_match;

  // std::slice text_match;
  // std::slice query_match;

  // match constructor

  match null_match() {
    return {
      .query_str_index = -1,
      .text_str_index = -1,
      .text_char_index = -1,
      .match_length = -1,
      .beyond_txt = false,
      .str = "",
    };
  }
};

std::ostream &operator<<(std::ostream &os, const junctions::match &m);

} // namespace junctions

// TODO: replace with std::slice
struct slicex {
  size_t start;
  size_t length;
};

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

/* Budowa drzewa sufiksowego algorytmem Ukkonena
 * (z przykladem wyszukiwania w nim slow)
 * Zlozonosc: O(n).
 */

struct STedge;
struct STvertex {
  map<char, STedge> g; /* edges to children */
  STvertex *f;         /* suffix link */
  // suffix number
  // (0 is an inch word, -1 means that the vertex is not a leaf)
  /* numer sufiksu (0 to cale slowo, -1 oznacza ze wierzcholek
   * nie jest lisciem) */
  int numer;
  int string_id;
};

struct STedge {
  int l, r; /* x[l]..x[r] is a piece of text that represents an edge */
  STvertex *v;
};



STvertex *Create_suffix_tree(const char *x, int n);
vector<int> FindEndIndexes(const char *query, STvertex *current_vertex, const char *x);
junctions::query_result FindEndIndexesTwo(const char *query, STvertex *current_vertex, const char *x);
void update_leaves(STvertex *current_vertex, std::vector<slicex> const *text_offsets);

// TODO: move graph stuff here

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
  std::vector<std::vector<slicex>> str_slices; 
  std::unordered_set<size_t> stops;
  std::unordered_set<size_t> starts;
  size_t size;
  size_t epsilons; // number of epsilons
  size_t m; // number of strings
  size_t length;
};

// TODO: reorder
void gen_suffix_tree(
    EDS const &eds,
    std::vector<std::pair<STvertex, std::string>> *suffix_trees);

struct LinearizedEDS {
  std::string str;
  std::vector<std::vector<int>> prev_chars;
};

struct cell {
  int j;
  int i;
};

bool operator<(const cell &lhs, const cell &rhs);

bool operator==(const cell &lhs, const cell &rhs);

std::ostream & operator<<(std::ostream &os, const cell &value);

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

bool operator==(const suffix &lhs, const suffix &rhs);

class Hasher {
public:
  // We use predefined hash functions of strings
  // and define our hash function as XOR of the
  // hash values.
  size_t operator()(const cell &c) const {
    return (std::hash<int>()(c.i)) ^ (std::hash<int>()(c.j));
  }

  size_t operator()(const suffix &s) const {
    return (std::hash<int>()(s.query_letter_idx)) ^
           std::hash<int>()(s.str_idx) ^ (std::hash<int>()(s.start_idx));
  }
};

struct spread {
  int start;
  int len;
};


std::ostream &operator<<(std::ostream &os, const spread &value);

//

struct match_info {
  int str_idx;
  int start_idx;
};



bool operator==(const match_info &lhs, const match_info &rhs);


std::ostream &operator<<(std::ostream &os, const match_info &value);

//

struct match_data {
  int text_str_idx;
  int text_start_idx;

  int query_str_idx;
  int query_start_idx;

  int length; // also the length of the match

  /*
  match_data() {
    this->text_str_idx = -1;
    this->text_start_idx = -1;
    this->query_str_idx = -1;
    this->query_start_idx = -1;
    this->length = -1;
  }

  match_data(int  ){}
*/

  bool is_exp_exp() const { return query_start_idx == 0 && text_start_idx == 0; }

  bool is_imp_exp() const { return query_start_idx > 0 && text_start_idx == 0; }

  bool is_exp_imp() const { return query_start_idx == 0 && text_start_idx > 0; }
};

const match_data INVALID_MATCH = match_data{.text_str_idx = -1,
                                            .text_start_idx = -1,
                                            .query_str_idx = -1,
                                            .query_start_idx = -1,
                                            .length = -1};



bool operator==(const match_data &lhs, const match_data &rhs);

std::ostream &operator<<(std::ostream &os, const match_data &value);

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

enum witness {shortest, longest};

  //std::ostream &operator<<(std::ostream &os, const core::witness &value);


enum arg {check_intersection, compute_graph, info};

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
  //
  arg task;

  // we assume input is in eds?
  std::vector<std::pair<file_format, std::string>> input_files;


  bool output_dot;
  bool size_of_multiset;
  // witness
  bool compute_witness;
  witness witness_choice;
  // matching stats
  bool compute_match_stats;
  int match_stats_str;
  int match_stats_letter_idx;
};
}

#endif
