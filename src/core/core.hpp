#ifndef CORE_HPP
#define CORE_HPP

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <set>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <chrono>
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
#include <unordered_set>
#include <unordered_map>

#include "../eds/eds.hpp"
#include "./constants.hpp"



namespace match_st {
/*
Suffix Tree
-----------
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

/* Suffix tree construction using Ukkonen's algorithm
 * (with an example of searching for a word)
 * Complexity: O(n).
 */

struct STedge;
struct STvertex {
  std::map<char, STedge> g; /* edges to children */
  STvertex *f;         /* suffix link */
  // suffix number
  // (0 is an inch word, -1 means that the vertex is not a leaf)
  int number;
  int string_id;
};

struct STedge {
  int l, r; /* x[l]..x[r] is a piece of text that represents an edge */
  STvertex *v;
};

class LeafData {
  std::size_t str_idx; // text str index

  // the match position in the conactenated string including $
  std::size_t char_idx;
public:
  // constructor(s)
  LeafData(std::size_t s, std::size_t c): str_idx(s), char_idx(c) {}

  // getters
  std::size_t get_char_idx() const { return this->char_idx; }
  std::size_t get_txt_str_idx() const { return this->str_idx; }

  // modifiers
  std::size_t& get_txt_char_idx_mut() { return this->char_idx; }

  // setters
  void set_char_idx(std::size_t i) { this->char_idx = i; }
};


class STQueryResult : private LeafData {
  bool beyond_text; // there was a match but the query was longer than the text
  std::size_t match_length; // the length of the match

public:
  // constructor(s)
  STQueryResult(bool beyond_text, std::size_t match_length, LeafData&& l_data)
    : LeafData(std::move(l_data)),
      beyond_text(beyond_text),
      match_length(match_length) {}

  STQueryResult(bool beyond_text, std::size_t match_length, LeafData& l_data)
    : LeafData(std::move(l_data)),
      beyond_text(beyond_text),
      match_length(match_length) {}

  STQueryResult(bool beyond_text, std::size_t match_length, std::size_t str_idx, std::size_t char_idx)
    : LeafData(str_idx, char_idx),
      beyond_text(beyond_text),
      match_length(match_length) {}

  // getters
  bool is_beyond_txt() const { return this->beyond_text; }
  std::size_t get_match_length() const { return this->match_length; }

  // modifiers
  std::size_t& get_match_length_mut() { return this->match_length; }

  //
  using LeafData::get_char_idx;
  using LeafData::get_txt_char_idx_mut;
  using LeafData::get_txt_str_idx;
};

bool operator==(const STQueryResult& lhs, const STQueryResult& rhs);
// operator < for STQueryResult
bool operator<(const STQueryResult& lhs, const STQueryResult& rhs);

struct internal_st_vertex {
  STvertex *vertex;
  std::size_t depth;
};

// operator  < for internal_st_vertex
bool operator<(const internal_st_vertex& lhs, const internal_st_vertex& rhs);

struct meta_st {
  std::string text;
  match_st::STvertex* root;
  std::map<std::size_t, std::set<internal_st_vertex>> marked_nodes;
};

/**
 * @brief Find all occurrences of the query in the suffix tree
 *
 * if end_in_imp_imp is true, a query in which two prefixes match should return as successful
 *
 * @param query the query
 * @param current_vertex the current vertex
 * @param x the text
 * @param end_in_imp_imp if true, the query must end in an implicit node
 * @return a vector of STQueryResult
 */
std::vector<match_st::STQueryResult> FindEndIndexes(const char *query,
                                                    STvertex *current_vertex,
                                                    const char *x,
                                                    bool end_in_imp_imp = false);

std::vector<match_st::STQueryResult> FindEndIndexes_(const char *query,
                                                     const internal_st_vertex &root,
                                                     const char *text,
                                                     std::map<std::size_t, std::set<internal_st_vertex>> &marked_nodes,
                                                     std::size_t qry_letter_idx,
                                                     bool end_in_imp_imp = false);

STvertex *Create_suffix_tree(const char *x, int n);

void gen_suffix_tree(eds::EDS &eds, std::vector<std::pair<STvertex, std::string>> *suffix_trees);

// a suffix tree real root the text string and internally marked nodes from
// previous searches

void gen_suffix_tree_(eds::EDS &eds, std::vector<meta_st> *suffix_trees);

void update_leaves(STvertex *current_vertex,
                   std::vector<eds::slice_eds> const &text_offsets,
                   eds::EDS& eds,
                   std::size_t letter_idx);
} // namespace match_st

namespace core {
typedef std::vector<std::vector<bool>> bool_matrix;
bool_matrix gen_matrix(size_t rows, size_t cols);

// C++ timer
typedef std::chrono::high_resolution_clock Time;

enum file_format {
  msa,    // msa in PIR format
  eds,    // eds file
  unknown //
};

enum ed_string_e {
  t1, // w or t1
  t2, // q or t2
};

std::ostream &operator<<(std::ostream &os, const ed_string_e &value);

// for use with intersection check
enum algorithm {
  naive,    //
  improved, //
  both      //
};

enum witness {shortest, longest};

enum task {
  check_intersection, // do EDSI TODO: rename?
  compute_graph,      // compute the intersection graph
  info,               // print info about the eds file
  unset               // default value
};

/**
 * @brief parameters for cli args
 */
struct AppConfig {
  algorithm algo;
  unsigned char v;         // verbosity
  std::string t1_file_path; // reference sequence(s)
  file_format t1_format;
  std::string t2_file_path; // query sequence(s)
  file_format t2_format;
  //
  task t;

  // we assume input is in eds?
  // TODO: make file format and string a struct
  std::vector<std::pair<file_format, std::string>> input_files;

  bool output_dot;
  bool size_of_multiset;
  // witness
  bool w;
  witness witness_choice;
  // matching stats
  bool compute_match_stats;
  int match_stats_str; // TODO: use enum
  int match_stats_letter_idx; // TODO: use size_t
  bool compute_match_stats_avg;
  bool compute_dist;
  bool compute_similarity;
  int constraint;

  AppConfig();

  // task
  task get_task() const;
  void set_task(task tsk);
  // is the current task tsk?
  bool is_task(task tsk);

  // algo
  // for intersection
  algorithm get_algo() const;
  void set_algo(algorithm a);
  // is the current task tsk?
  bool is_algo(algorithm a);

  // fp (file path)
  std::pair<file_format, std::string> get_w_fp() const;
  std::pair<file_format, std::string> get_q_fp() const;

  void set_witness(bool b);
  void set_verbosity(unsigned char v);
  //void set_verbosity(int v);

  bool gen_dot() const;
  bool compute_witness() const;
  bool multiset() const;
  enum witness get_witness_choice() const;
  unsigned char verbosity() const;
};

// TODO call this an eds match and move to core?
struct EDSMatch {
  std::size_t query_str_index;
  std::string str;
  // in this case we modify the char idx in q_res.l_data.char_idx
  // to be actual position in the text string
  // match_st::STQueryResult &q_res;

  match_st::STQueryResult q_res;

public:
  // constructor(s)
  EDSMatch(std::size_t query_str_index, std::string str, match_st::STQueryResult &&q_res)
    : query_str_index(query_str_index),
      str(str),
      q_res(std::move(q_res)) {}

  EDSMatch(std::size_t query_str_index, std::string str, match_st::STQueryResult& q_res)
    : query_str_index(query_str_index),
      str(str),
      q_res(std::move(q_res)) {}

  EDSMatch(std::size_t qry_str_index, std::size_t txt_str_idx,
           std::size_t txt_char_idx, std::string str, bool beyond_txt,
           std::size_t m_len)
    : query_str_index(qry_str_index),
      str(str),
      q_res(match_st::STQueryResult(beyond_txt,
                                    m_len,
                                    txt_str_idx,
                                    txt_char_idx)) {}

  //
  std::size_t get_qry_str_idx() const { return this->query_str_index; }
  std::string const& get_str() const { return this->str; }

  // modifiers
  std::string& get_str_mut() { return this->str; }

  /*
   * from LeafData
   */

  // getters
  std::size_t get_char_idx() const { return this->q_res.get_char_idx(); }
  std::size_t get_txt_str_idx() const { return this->q_res.get_txt_str_idx(); }

  // modifiers
  std::size_t& get_txt_char_idx_mut() { return this->q_res.get_txt_char_idx_mut(); }

  // setters
  //void set_char_idx(std::size_t i) { this->char_idx = i; }

  /*
   * from STQueryResult
   */
  bool is_beyond_txt() const { return this->q_res.is_beyond_txt(); }
  std::size_t get_match_length() const { return this->q_res.get_match_length(); }

  // modifiers
  std::size_t& get_match_length_mut() { return this->q_res.get_match_length_mut(); }
};




void perform_matching(eds::EDS &txt_eds, std::size_t txt_letter_idx,
                      std::pair<match_st::STvertex, std::string> *text, // TODO: rename param
                      std::vector<std::string> const &queries,
                      std::vector<EDSMatch> *candidate_matches, bool end_in_imp_imp = false);


void perform_matching_(eds::EDS &txt_eds,
                       std::size_t txt_letter_idx,
                       std::size_t qry_letter_idx,
                       match_st::meta_st& meta_st_,
                      std::vector<std::string> const &queries,
                      std::vector<EDSMatch> *candidate_matches, bool end_in_imp_imp = false);

void mark_query_nodes(eds::EDS &qry_eds,
                      std::size_t qry_letter_idx,
                      std::size_t txt_letter_idx,
                      match_st::meta_st& meta_st_,
                      std::vector<EDSMatch>& candidate_matches );

// TODO: move to utils
void join(const std::vector<std::string> &v, char c, std::string &s);
std::string indent(int level);

// TODO [c] move to core::constants
const std::string unicode_eps = "\u03B5";
const std::string unicode_sub_1 = "\u2081";
const std::string unicode_sub_2 = "\u2082";

const std::string T_1 = "T\u2081";
const std::string T_2 = "T\u2082";
const std::string N_1 = "N\u2081";
const std::string N_2 = "N\u2082";
const std::string q_0 = "q\u2080";
const std::string q_a = "q\u2090";

const char string_separator = '$';
const char terminator_char = '_';
} // namespace core
#endif
