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

#include "../eds/eds.hpp"


namespace n_core {
typedef std::vector<std::vector<bool>> bool_matrix;
bool_matrix gen_matrix(size_t rows, size_t cols);

enum file_format {
  msa, // msa in PIR format
  eds, // eds file
  unknown
};


enum ed_string { w, q };

std::ostream &operator<<(std::ostream &os, const ed_string &value);

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

  Parameters();
};

 // TODO: remove
struct match_locus {
  int string_index; // rename to str_index
  int char_index;   // is this always zero?
                    // candidate active suffix match

  bool candidate_asm() { return !(this->char_index == 0); }
};

bool operator<(const match_locus &lhs, const match_locus &rhs);
typedef match_locus locus;


// TODO remove

// generalization of query result and match locus above
// replace query_result an match_locus with this
struct extended_match {
  bool beyond_text; // there was a match but the query was longer than the text
  int match_length; // the length of the match
  int str_idx; // the index of the text string

  // the match position in the conactenated string ignoring $
  // TODO: make this char idx in the actual txt string
  int chr_idx;
};

std::ostream &operator<<(std::ostream &os, const extended_match &r);

bool operator==(const extended_match &lhs, const extended_match &rhs);


} // namespace core


namespace match_st {
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

typedef std::vector<int> VI;
typedef std::pair<int, int> PII;
typedef long long ll;
typedef std::vector<std::string> VS;

ll nwd(ll a, ll b);

VS parse(std::string s);

int toi(char ch);

int chg(char ch);

int los(int m);

/* Budowa drzewa sufiksowego algorytmem Ukkonena
 * (z przykladem wyszukiwania w nim slow)
 * Zlozonosc: O(n).
 */

struct STedge;
struct STvertex {
  std::map<char, STedge> g; /* edges to children */
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

std::vector<n_core::extended_match> FindEndIndexes(const char *query,
                                                   STvertex *current_vertex,
                                                   const char *x);
  
STvertex *Create_suffix_tree(const char *x, int n);

void gen_suffix_tree_new(eds::EDS &eds,
                         std::vector<std::pair<STvertex, std::string>> *suffix_trees);
void update_leaves(STvertex *current_vertex,
                   std::vector<eds::slice_eds> const &text_offsets);

} // namespace match


namespace n_junctions {

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

void join(const std::vector<std::string> &v, char c, std::string &s);

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

std::ostream &operator<<(std::ostream &os, const graph_slice &s);

struct match {
  int query_str_index;
  // no need for query_char_idx bexause it's always 0

  int text_str_index;
  int text_char_index;

  int match_length;
  bool beyond_txt;

  std::string str;
};

std::ostream &operator<<(std::ostream &os, const match &m);

// new
void perform_matching(std::vector<std::string> const &queries,
                      std::pair<match_st::STvertex, std::string> *text,
                      std::vector<match> *candidate_matches,
                      n_core::Parameters const &parameters);

} // namespace junctions


#endif
