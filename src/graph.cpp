#include <climits>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

#include "./parser.hpp"
#include "core.hpp"
#include "utils.hpp"

using namespace std;

namespace graph {
/**
 * edge struct to represent a weighted edge
 */
struct Edge {
  int dest;   // destination vertex
  int weight; // weight of the edge, length of a string in characters.
  std::string str;
  bool is_eps; // is the edge an epsilon/hybrid edge
};

bool operator==(const Edge &lhs, const Edge &rhs) {
  return std::tie(lhs.dest, lhs.weight, lhs.str, lhs.is_eps) == std::tie(rhs.dest, rhs.weight, rhs.str, rhs.is_eps);
}

bool operator<(const Edge &lhs, const Edge &rhs) {
  return std::tie(lhs.dest, lhs.weight, lhs.str, lhs.is_eps) < std::tie(rhs.dest, rhs.weight, rhs.str, rhs.is_eps);
}

std::ostream &operator<<(std::ostream &os, const Edge &e) {
  os << "Edge {"
     << "dest: " << e.dest << " qry start: " << e.str
     << "len: " << e.weight << " str: " << e.is_eps << "}" << std::endl;
  return os;
}

// TODO: use unordered set
struct Vertex {
  std::set<Edge> incoming;
  std::set<Edge> outgoing;
  /*
   * invalid -1
   * exp exp 0
   * exp imp 1
   * imp exp 2
   * imp imp 3
   */
  int vertex_type; 

  // initialize a struct with default values
  Vertex(): incoming(std::set<Edge>{}), outgoing(std::set<Edge>{}), vertex_type(-1) {}
};

/**
 *
 *
 *
 * adj adjacency list to store edges
 */
struct Graph {
  int V;                    // number of vertices
  int N_1;                  // yyy
  int N_2;                  // xxx
  set<int> q_0; // start node
  vector<Edge> q_a; // accept node
  vector<Vertex> adj; // adjacency list to store edges

  // constructor to initialize the graph
  Graph(int N_1, int N_2) {
    this->N_1 = N_1;
    this->N_2 = N_2;
    this->V = N_1 * N_2;
    adj.resize(V);
  }

  // x in [1 - n1 -1], also l
  // y in [1 - n2 -1], also k
  // zero indexed
  int compute_index(int x, int y, int i, int j) { return ((y+j)*N_1) + x+i; }

  bool is_exp_exp(int idx) { return this->adj[idx].vertex_type == 0; }

  bool is_exp_imp(int idx) { return this->adj[idx].vertex_type == 1; }

  bool is_imp_exp(int idx) { return this->adj[idx].vertex_type == 2; }

  bool is_imp_imp(int idx) { return this->adj[idx].vertex_type == 3; }

  int multiset_size() {
    std::vector<int> dp_table(this->V, 0);
    int start_node, stop_node;
    start_node = 0;
    stop_node = this->V-1;
    dp_table[start_node] = 1;

    std::set<Edge> out;

    int current_node = start_node;

    std::stack<int> to_visit;
    std::set<int> visited;

    to_visit.push(current_node);

    while (!to_visit.empty()) {
      current_node = to_visit.top();
      to_visit.pop();

      if (visited.count(current_node) > 0) { continue; }

      out = this->adj[current_node].outgoing;
      for (auto e: out) {
        dp_table[e.dest] +=  dp_table[current_node];
        to_visit.push(e.dest);
      }
    }

    return dp_table[stop_node];
  }

  /**
   * what is the furthest path from this given start node
   *
   *
   * @param[in] start_node_idx
   * @return 
   */
  int longest_frm_start(int start_node_idx) {
    int stop_node = this->adj.size();

    auto compute_table_idx = [&start_node_idx](int gr_idx) -> int { return gr_idx - start_node_idx; };
    auto compute_graph_idx = [&start_node_idx](int tbl_idx) -> int { return tbl_idx + start_node_idx + 1; };
    std::vector<int> dists(this->V, INT_MIN);

    int max = 0;

    std::stack<int> to_visit;
    std::set<int> visited;
    std::set<Edge> out;
    int current_dist;

    int current_node = start_node_idx;
    dists[current_node] = 0;
    to_visit.push(current_node);

    while (!to_visit.empty()) {
      current_node = to_visit.top();
      to_visit.pop();
      out = this->adj[current_node].outgoing;

      if (visited.count(current_node) > 0) {
        continue;
      }

      current_dist = dists[current_node];

      for (auto e : out) {
        if (dists[compute_table_idx(e.dest)] < current_dist + e.dest) {
          dists[compute_table_idx(e.dest)] = current_dist + e.dest;
          if (max < current_dist + e.dest) { max = current_dist + e.dest; }
        }

        to_visit.push(e.dest);
      }

      visited.insert(current_node);
    }

    return max;
  }

  int witness(int start_node_idx, int stop_node_idx) {
    auto tbl_idx = [&start_node_idx](int gr_idx) -> int { return gr_idx - start_node_idx; };
    auto gr_idx = [&start_node_idx](int tbl_idx) -> int { return tbl_idx + start_node_idx + 1; };

    // int size = stop_node_idx - start_node_idx + 1;
    std::vector<int> dists(this->V, INT_MIN);

    std::stack<int> to_visit;
    std::set<int> visited;
    std::set<Edge> out;
    int current_dist;

    int current_node = start_node_idx;
    dists[current_node] = 0;
    to_visit.push(current_node);

    while (!to_visit.empty()) {
      current_node = to_visit.top();
      to_visit.pop();
      out = this->adj[current_node].outgoing;

      if (visited.count(current_node) > 0) { continue; }

      current_dist = dists[current_node];

      for (auto e : out) {
        if (dists[tbl_idx(e.dest)] < current_dist + e.weight ) {
          dists[tbl_idx(e.dest)] = current_dist + e.weight;
        }

        to_visit.push(e.dest);
      }
      visited.insert(current_node);
    }

    return dists[tbl_idx(stop_node_idx)];
  }

  int djikstra(int start_node_idx, int stop_node_idx) {
    auto tbl_idx = [&start_node_idx](int gr_idx) -> int { return gr_idx - start_node_idx; };
    auto gr_idx = [&start_node_idx](int tbl_idx) -> int { return tbl_idx + start_node_idx + 1; };

    int size = stop_node_idx - start_node_idx + 1;
    std::vector<int> dists(this->V, INT_MAX);
    dists[0] = 0;

    int current_node = start_node_idx;
    int current_dist;
    std::pair <int,int> min; // node idx in the graph and dist

    while (current_node != stop_node_idx) {
      std::set<Edge> out = this->adj[current_node].outgoing;
      min = std::make_pair(current_node, INT_MAX);

      current_dist = dists[current_node];

      // TODO: handle epsilon
      for (auto e: out) {
        // TODO: merge with relax
        // find min outgoing

        int weight = e.is_eps ? 0 : e.weight ;

        if ((current_dist + weight) < min.second ) {
          min = std::make_pair(e.dest, current_dist + weight);
        }

        // relax
        if (dists[tbl_idx(e.dest)] > current_dist +weight) {
          dists[tbl_idx(e.dest)] = current_dist + weight;
        }
      }

      current_node = min.first;
    }

    return dists[tbl_idx(stop_node_idx)];
  }

  // function to add an edge to the graph
  // src is T_1
  // src is T_2
  void addEdge(int N_1,
               int N_2,
               std::pair<int, int> i_boundary,
               std::pair<int, int> j_boundary,
               std::pair<junctions::match_type, junctions::match_type> w_m,
               std::pair<junctions::match_type, junctions::match_type> q_m,
               int i,
               int j,
               int weight,
               std::string str,
               core::Parameters const &parameters,
               int eps_side = 0
               ) {
    if (parameters.verbosity > 2) {
      std::cerr << utils::indent(1) << "DEBUG, [Graph::g.addEdge]" << std::endl;
    }

    int stop, start;
    int l, k, l_prime, k_prime;

    if (i > 0) {
      l = w_m.first == junctions::match_type::exp ? i_boundary.first - 1 : N_1;
    } else {
      l = w_m.first == junctions::match_type::exp ? i_boundary.first : N_1;
    }

    if (j > 0) {
      k = q_m.first == junctions::match_type::exp ? j_boundary.first -1 : N_2;
    } else {
      k = q_m.first == junctions::match_type::exp ? j_boundary.first : N_2;
    }

    start = compute_index(l, k, i, j);

    if ((i == 0 && j == 0) &&
        q_m.first == junctions::match_type::exp && w_m.first == junctions::match_type::exp) {
      this->q_0.insert(start);
    }

    l_prime = w_m.second == junctions::match_type::exp ? i_boundary.second + 1 : N_1 + weight;
    k_prime = q_m.second == junctions::match_type::exp ? j_boundary.second + 1 : N_2 + weight;

    if (eps_side == 1) {
      k_prime = k;
    }

    if (eps_side == 2) {
      
      l_prime = l;
    }

    stop = compute_index(l_prime, k_prime, i, j);

    bool is_eps = (eps_side == 1 || eps_side == 2) ? true : false;

    if (is_eps ) { weight = 0; str = "";}

    Edge e_rev = {.dest = start, .weight = weight, .str = str,  .is_eps = is_eps};
    Edge e = {.dest = stop, .weight = weight, .str = str, .is_eps = is_eps};

    adj[start].outgoing.insert(e);
    adj[stop].incoming.insert(e_rev);

    int s_typ, e_typ;

    if (w_m.first == junctions::match_type::exp && q_m.first == junctions::match_type::exp) {
      s_typ = 0;
    } else if (w_m.first == junctions::match_type::exp && q_m.first == junctions::match_type::imp) {
      s_typ = 1;
    } else if (w_m.first == junctions::match_type::imp && q_m.first == junctions::match_type::exp) {
      s_typ = 2;
    } else if (w_m.first == junctions::match_type::imp && q_m.first == junctions::match_type::imp) {
      s_typ = 3;
    } else {
      s_typ = -1;
    }

    if (w_m.second == junctions::match_type::exp && q_m.second == junctions::match_type::exp) {
      e_typ = 0;
    } else if (w_m.second == junctions::match_type::exp && q_m.second == junctions::match_type::imp) {
      e_typ = 1;
    } else if (w_m.second == junctions::match_type::imp && q_m.second == junctions::match_type::exp) {
      e_typ = 2;
    } else if (w_m.second == junctions::match_type::imp && q_m.second == junctions::match_type::imp) {
      e_typ = 3;
    } else {
      e_typ = -1;
    }

    if (parameters.verbosity > 3) {
      std::cerr << utils::indent(2) << "(N_1, N_2) (" << N_1 << ", " << N_2 << ")"
                << std::endl
                << utils::indent(2) << "(l, l') (" << l << ", " << l_prime << ")"
                << std::endl
                << utils::indent(2) << "(k, k') (" << k << ", " << k_prime << ")"
                << std::endl
                << utils::indent(2) << "start " << start
                << std::endl
                << utils::indent(2) << "stop " << stop
                << std::endl
                << utils::indent(2) << "weight " << weight
                << std::endl
                << utils::indent(2) << "str " << str
                << std::endl
                << utils::indent(2) << "(" << w_m.first << ", " << q_m.first << ") (" << w_m.second << ", " << q_m.second << ")"
                << std::endl
                << utils::indent(2) << "s " << s_typ << " e " << e_typ
                << std::endl;
    }

    adj[start].vertex_type = s_typ;
    adj[stop].vertex_type = e_typ;
  }

  void dbg_print(int indent_level=1) {
    std::cerr << utils::indent(indent_level) << "Graph {"
              << std::endl
              << utils::indent(indent_level + 1) << "size: " << this->V
              << std::endl
              << utils::indent(indent_level) << "}"
              << std::endl;
  }

  void print_dot() {
    auto print_idx = [&](int idx) -> string {
      if (this->q_0.count(idx) > 0) { return "q_0"; }
      if (idx == this->V - 1) { return "q_a"; }
      return std::to_string(idx);
    };

    std::cout
        << "digraph G {\n"
        << "\trankdir = LR;\n"
        << "\tnode[shape = circle];\n";

    std::cout << "\t" << "q_0" << "[color=\"green\"];" << std::endl;
    std::cout << "\t" << "q_a" << "[color=\"green\"];" << std::endl;

    for (int i = 0; i < this->V; i++) {

      if (this->q_0.count(i) > 0) {continue;}

      if (this->adj[i].vertex_type == 0 && i != this->V-1 ) {
        std::cout << "\t" << std::to_string(i) << "[color=\"blue\"];" << std::endl;
      } else if (this->adj[i].vertex_type == 1 || this->adj[i].vertex_type == 2) {
        std::cout << "\t" << std::to_string(i) << "[color=\"orange\"];" << std::endl;
      } else if (this->adj[i].vertex_type == 3) {
        std::cout << "\t" << std::to_string(i) << "[color=\"red\"];" << std::endl;
      } else {
      }
    }

    for (auto v: this->adj) {
      if (v.vertex_type == 0) {

      } else if (v.vertex_type == 1 || v.vertex_type == 2) {
        }
    }

    /*
    for (auto o: this->q_0 ){
      std::cout << "\t" << "q_0 -> " << std::to_string(o.dest) << "\n";
    }
    */

    // print nodes
    std::string s;
    for (int i=0; i < this->V; i++){


      Vertex v = this->adj[i];

      // unreachable
      if (v.incoming.empty() && v.outgoing.empty()) {
        if (false) { std::cout << "\t" + print_idx(i) + ";\n"; }
        continue;
      }

      // std::cerr << utils::indent(2) << "n: " << i << " -> ";

      std::string label, to;
      for (auto e : v.outgoing) {

        // std::cerr << e.dest;

        if (e.is_eps) {
          // label = " \"(" + e.str + ", \u03B5, " + std::to_string(e.weight) + ")\"";
          label = " \"(\u03B5, " + std::to_string(e.weight) + ")\"";
        } else {
          label = " \"(" + e.str + ", " + std::to_string(e.weight) + ")\"";
        }

        std::cout << "\t" << print_idx(i) << " -> " << print_idx(e.dest)
                  << " [label=" << label
                  << ", weight=" << std::to_string(e.weight)
                  << "];\n";
      }
      //std::cerr << std::endl;
    }

    std::cout << "}" << std::endl;
  }
};

/**
 * @param[out] w_suffix trees: a triple containing the vertex
 */
void gen_suffix_tree(EDS const &eds,
                     size_t len,
                     std::vector<std::pair<STvertex, std::string>> *w_suffix_trees
                     ) {
  size_t i = 0;
  while (i < len) {
    std::vector<std::string> i_letter = eds.data[i].data;
    std::vector<slicex> const* str_slices = &eds.str_slices[i];
    std::string text;
    for (auto i_str : i_letter) {
      text.append(i_str);
    }                    // concat the strings
    text.push_back('_'); // add a terminator char

    // Create the suffix tree
    STvertex* root = Create_suffix_tree(text.c_str(), text.length());
    update_leaves(root, str_slices);
    w_suffix_trees->push_back(std::make_pair(*root, text));

    ++(i);
  }
};


/**
 *
 *
 * @param[in]  queries           queries
 * @param[in]  text              text
 * @param[out] candidate_matches matches_found in the context of the degenerate letter and not N
*/
void match(std::vector<string> const &queries,
           std::pair<STvertex, std::string> *text,
           std::vector<junctions::match> *candidate_matches,
           core::Parameters const &parameters
           ) {
  if (parameters.verbosity > 2) {
    std::cerr << utils::indent(1) << "DEBUG, [graph::match]" << std::endl;
  }

  // std::size_t query_count = queries.size();
  // match_info x = match_info{.str_idx = -1, .start_idx = -1};
  // std::vector<std::vector<match_info>> foobar;
  // foobar.reserve(queries.size());

  // matches for a given query, may or may not be valid
  // string
  // match index pair
  // std::vector<std::pair<int, int>> match_positions;
  junctions::query_result match_positions;
  // std::vector<junctions::match> candidate_meatches;

  // std::vector<match_info> matches_found;
  // matches found for all queries
  slicex candidate_txt_str;

  // int last_index = text_offsets->back().start +
  // text_offsets->back().length;

  for (int qry_str_idx = 0; qry_str_idx < queries.size(); qry_str_idx++) {
    std::string qry_str = queries[qry_str_idx];
    match_positions = FindEndIndexesTwo(qry_str.c_str(), &text->first, text->second.c_str());

    if (match_positions.is_miss()) { continue; }

    int match_len = match_positions.match_length;

    // The query at idx has at least one match match
    // a vector of match locus
    for (auto match_pos : match_positions.results) {

      if (parameters.verbosity > 3) {
        std::cerr << utils::indent(2)
                  << "qry " << qry_str
                  << " txt " << text->second
                  << " txt str idx " << match_pos.string_index
                  << "txt start" << match_pos.char_index << std::endl;
      }

      candidate_matches->push_back(
          junctions::match{.query_str_index = qry_str_idx,
                           .text_str_index = match_pos.string_index,
                           .text_char_index = match_pos.char_index,
                           .match_length = match_len,
                           .beyond_txt = match_positions.beyond_text,
                           .str = qry_str.substr(0, match_len)});
    }
  }
}

std::pair<int, int> compute_letter_boundaries(std::vector<span> const &offsets) {
  int left = offsets.front().start;
  int right = offsets.back().stop;

  return std::make_pair(left, right);
}

/**
 *
 * Validate matches
 * find out where they start and end within the query and the text
 *
 * active suffixes exist in N
 *
 * txt_slices exist in l start and length
 * txt_offsets exist in N start and stop indexes of a str in N
 * qry_offsets exist in N start and stop indexes of a str in N
 * return
*/
void filter_matches(std::vector<junctions::match> const &candidate_matches,
                    std::vector<slicex> const &txt_slices,
                    std::vector<span> const &txt_offsets,
                    std::vector<span> const &qry_offsets,
                    matrix *active_suffixes, int qry_letter_idx,
                    std::vector<junctions::graph_slice> *v,
                    core::Parameters const &parameters ) {

  if (parameters.verbosity > 2) {
    std::cerr << utils::indent(1) << "DEBUG, [graph::filter_matches]"
              << std::endl;
  }


  //int txt_start_in_N = txt_offsets[0].start;
  auto in_txt_N = [&](int k) -> int { return txt_offsets[0].start + k; };
  auto in_txt_N_2 = [&](int idx, int k) -> int { return txt_offsets[idx].start + k; };
  //auto in_qry_N = [&](int l) -> int { return qry_offsets[0].start + l; };

  // std::vector<junctions::graph_slice> v;

  for (auto candiate_match : candidate_matches) {

    slicex txt_slice = txt_slices[candiate_match.text_str_index];

    /*
      candiate_match.text_char_index: the start of the position of text within the concatenated txt
     */

    // this is the start position of the char within the specific string in the txt set
    int match_start_in_txt = candiate_match.text_char_index -  txt_slice.start;



    /*
      evaluate the start of the match
      ===============================
     */

    // explicit or implicit match
    junctions::match_type t_m_start, t_m_stop, q_m_start, q_m_stop;

    // is the active suffix valid?
    bool valid_as, valid_txt_start;
    valid_as = valid_txt_start = false;

    // TODO: rename t_start in N to something like match_start_in_N
    int t_start_in_N = in_txt_N(candiate_match.text_char_index);

    if (parameters.verbosity > 3) {
      std::cerr << utils::indent(2) << "txt_str_idx"
                << candiate_match.text_str_index << "start char in concat l"
                << candiate_match.text_char_index << " start char in l  "
                << match_start_in_txt << " char start in N (not concat) "
                << t_start_in_N << std::endl;
    }

    // int txt_end = txt_slice.start + txt_slice.length;

    // the match starts within a string in the text
    // second condition because actv suff can only be extended ...
    if (match_start_in_txt > 0 && qry_letter_idx > 0) {
        // is valid active suffix
        // int in_N = in_txt_N(candiate_match.text_char_index); wrong
        valid_as = ((*active_suffixes)[qry_letter_idx - 1][t_start_in_N] == 1);
    }

    // is an exp - exp match
    // or valid active suffix
    // txt start is not valid so skip
    if (match_start_in_txt > 0 && !valid_as) { continue; }



    if (match_start_in_txt == 0) {
      t_m_start = junctions::match_type::exp;
    } else {
      t_m_start = junctions::match_type::imp;
    }

    /*
      evaluate the END of the match
      ===============================
    */

    int match_end = -1; // TODO: declare earlier

    int m_len= -1; // the actual length of the match

    // where the match ends
    int candidate_match_end = match_start_in_txt + candiate_match.match_length;

    // where the matched string actually ends
    int txt_slice_end = txt_slice.start + txt_slice.length;

    if (parameters.verbosity > 3) {
      std::cerr << utils::indent(2) << candidate_match_end << " " << txt_slice_end
                << std::endl;
    }

    // find where the match ends within the txt string
    if (txt_slice.start + candidate_match_end >= txt_slice_end) {
      // the match went beyond the txt so we limit it to the end
      // a match that is beyond the text also lies here

      match_end = txt_slice_end; // actual idx is -1

      m_len = match_end - (txt_slice.start + match_start_in_txt);
      t_m_stop = junctions::match_type::exp;
    } else {
      // within the text
      match_end = candidate_match_end; // actual idx is -1
      m_len = candiate_match.match_length;

      t_m_stop = junctions::match_type::imp;

      // handle active suffix
      // this creates and active suffix
      // set match end as an active suffix
      int in_N = in_txt_N_2(candiate_match.text_str_index, match_end);
      // std::cerr << utils::indent(2) << "save as: " << qry_letter_idx << in_N;
      (*active_suffixes)[qry_letter_idx][in_N] = 1;
    }

    /*
      Handle query
      ============

      queries always start at 0 so we don't need to do any additional stuff
     */
    // determine match start and ends in N


    // in the query
    int q_start_in_N = qry_offsets[candiate_match.query_str_index].start;

    if (parameters.verbosity > 3) {
      std::cerr << utils::indent(2) << " q in N " << q_start_in_N << std::endl;
    }

    // std::make_pair(q_start_in_N, t_start_in_N, m_len);

    // the query is always spelt to the end or past it
    // TODO: handle spelling past the text
    // when the qry is spelt past the end??
    q_m_start = junctions::match_type::exp;
    q_m_stop = candiate_match.beyond_txt ? junctions::match_type::imp : junctions::match_type::exp;

    if (parameters.verbosity > 3) {
      std::cerr << utils::indent(2)
                << "beyond txt: " << candiate_match.beyond_txt
                << " (txt): " << t_m_start << ", " << t_m_stop
                << " (qry) " << q_m_start << ", " << q_m_stop
                << std::endl;
    }

    // TODO: replace with proper message
    if (m_len < 0) {
      std::cerr << "----" << std::endl;
      exit(1);
    }

      junctions::graph_slice s =
          junctions::graph_slice{.txt_start = t_start_in_N,
                                 .qry_start = q_start_in_N,
                                 .q_m = std::make_pair(q_m_start, q_m_stop),
                                 .t_m = std::make_pair(t_m_start, t_m_stop),
                                 .len = m_len,
                                 .str = candiate_match.str};

      if (parameters.verbosity > 3) { s.dbg_print(2); }

      // std::cerr << utils::indent(2) << s;

      v->push_back(s);
      /*
      v->push_back(junctions::graph_slice{.txt_start = t_start_in_N,
                                          .qry_start = q_start_in_N,
                                          .len = m_len,
                                          .str = candiate_match.str});
                                          */
  }

  // return v;
}

/**
 *
 * @param[in] g
 * @param[in] valid_matches
 *
 */
void create_edge(Graph *g,
                 std::vector<junctions::graph_slice> const &valid_matches,
                 int qry, std::pair<int, int> qry_boundary,
                 std::pair<int, int> txt_boundary, int i, int j,
                 core::Parameters const &parameters,
                 bool eps_edge = false) {
  if (parameters.verbosity > 2) {
    std::cerr << utils::indent(1) << "DEBUG, [graph::create_edge]"
              << std::endl;
  }

  int eps_side = 0;

  switch (qry) {
  case 1:
    // T_1 is the qry
    //int eps_side = 0;
    if (eps_edge) {eps_side = 2;}
    for (auto x : valid_matches) {
      g->addEdge(x.qry_start, x.txt_start, qry_boundary, txt_boundary,
                 x.q_m, x.t_m, i, j, x.len, x.str, parameters, eps_side);
    }
    break;
  case 2:
    // T_2 is the qry
    if (eps_edge) { eps_side = 1; }
    for (auto x : valid_matches) {
      g->addEdge(x.txt_start, x.qry_start, txt_boundary, qry_boundary, x.t_m,
                 x.q_m, i, j, x.len, x.str, parameters, eps_side);
    }
    break;
  default:
    break;
  };
}

/**
*
*
* @param[in] eds_w
* @param[in] eds_q
* @param[in] parameters
* return
*/
Graph compute_intersection_graph(EDS &eds_w, EDS &eds_q,
                                 core::Parameters const &parameters) {
  if (parameters.verbosity > 1) { std::cerr << "[graph::compute_intersection_graph]" << std::endl; }

  size_t size_w = eds_w.size;
  size_t size_q = eds_q.size;

  int len_w = eds_w.length;
  int len_q = eds_q.length;

  /*
    Generate suffix trees
    ---------------------
  */

  std::vector<std::pair<STvertex, std::string>> w_suffix_trees;
  w_suffix_trees.reserve(len_w);
  std::vector<std::pair<STvertex, std::string>> q_suffix_trees;
  q_suffix_trees.reserve(len_q);

  gen_suffix_tree(eds_w, len_w, &w_suffix_trees);
  gen_suffix_tree(eds_q, len_q, &q_suffix_trees);

  /*
   *
   */
  std::vector<junctions::match> candidate_matches;
  candidate_matches.reserve(100);

  // active suffixes in N_1
  matrix i_active_suffixes = utils::gen_matrix(len_q, size_w + eds_w.epsilons);
  // active suffixes in N_2
  matrix j_active_suffixes = utils::gen_matrix(len_w, size_q + eds_q.epsilons);

  // std::vector<std::vector<junctions::match>> matches_found;

  std::vector<junctions::graph_slice> valid_matches;
  valid_matches.reserve(1000);

  auto clean_up = [&candidate_matches, &valid_matches]() {
    candidate_matches.clear();
    valid_matches.clear();
  };

  /*
    Compute Graph
    -------------
  */

  Graph g = Graph(eds_w.size + eds_w.epsilons + len_w, eds_q.size + eds_q.epsilons + len_q);

  if (parameters.verbosity > 0) { g.dbg_print(1); }

  std::pair<int, int> qry_boundary;
  std::pair<int, int> txt_boundary;

  for (int i = 0; i < len_w; i++) {
    for (int j = 0; j < len_q; j++) {

      if (parameters.verbosity > 2) {
        std::cerr << std::endl
                  << utils::indent(1) << "i: " << i << " j: " << j << std::endl
                  << utils::indent(1) << "--------------" << std::endl;
      }

      // T_2[j] has epsilon
      // T_1[i] is the query
      if (eds_q.data[j].has_epsilon) {
        int eps_idx = eds_q.str_offsets[j].back().start;
        std::vector<span> i_offsets = eds_w.str_offsets[i];
        int str_count = eds_w.data[i].has_epsilon ? i_offsets.size() - 1 : i_offsets.size();

        for (int idx = 0; idx < str_count; idx++ ) {
          span offset = i_offsets[idx];
          int str_len = (1 + offset.stop) - offset.start;
          std::string str = eds_w.data[i].data[idx];

          valid_matches.push_back(junctions::graph_slice{
              .txt_start = eps_idx,
              .qry_start = (int)offset.start,
              .q_m = std::make_pair(junctions::match_type::exp, junctions::match_type::exp),
              .t_m = std::make_pair(junctions::match_type::exp, junctions::match_type::exp),
              .len = str_len,
              .str = str});
        }

        txt_boundary = compute_letter_boundaries(eds_q.str_offsets[j]);
        qry_boundary = compute_letter_boundaries(eds_w.str_offsets[i]);

        create_edge(&g, valid_matches, 1, qry_boundary, txt_boundary, i, j, parameters , true);
      }

      clean_up();

      // T_1[i] has epsilon
      // T_2[j] is the query
      if (eds_w.data[i].has_epsilon) {
        int eps_idx = eds_w.str_offsets[i].back().start;
        std::vector<span> j_offsets = eds_q.str_offsets[j];
        int str_count =
            eds_q.data[j].has_epsilon ? j_offsets.size() - 1 : j_offsets.size();

        for (int idx = 0; idx < str_count; idx++) {
          span offset = j_offsets[idx];
          int str_len = (1 + offset.stop) - offset.start;
          std::string str = eds_q.data[j].data[idx];

          valid_matches.push_back(junctions::graph_slice{
              .txt_start = eps_idx,
              .qry_start = (int)offset.start,
              .q_m = std::make_pair(junctions::match_type::exp, junctions::match_type::exp),
              .t_m = std::make_pair(junctions::match_type::exp, junctions::match_type::exp),
              .len = str_len,
              .str = str});
        }

        qry_boundary = compute_letter_boundaries(eds_q.str_offsets[j]);
        txt_boundary = compute_letter_boundaries(eds_w.str_offsets[i]);

        create_edge(&g, valid_matches, 2, qry_boundary, txt_boundary, i, j,
                    parameters, true);
      }

      clean_up();

      // Search for j_strs in T_1[i]
      // ---------------------------

      // Query => T_2[j]
      // Text => T_1[i]

      if (parameters.verbosity > 3) {
        std::cerr << "Query => T_2[" << j << "] Text => T_1[ " << i << "]"
                  << std::endl;
      }

        
      // perform matching
      match(eds_q.data[j].data, &w_suffix_trees[i], &candidate_matches,
            parameters);

      // filter matches
      filter_matches(candidate_matches, eds_w.str_slices[i],
                     eds_w.str_offsets[i], eds_q.str_offsets[j],
                     &i_active_suffixes, j, &valid_matches, parameters);

      qry_boundary = compute_letter_boundaries(eds_q.str_offsets[j]);
      txt_boundary = compute_letter_boundaries(eds_w.str_offsets[i]);

      // update edge
       create_edge(&g, valid_matches, 2, qry_boundary, txt_boundary, i, j,
                   parameters );

       // clean up
       clean_up();

       // Search for i_strs in T_2[j]
       // ---------------------------
       if (parameters.verbosity > 3) {}
       if (parameters.verbosity > 3) {
        std::cerr << "Query => T_1[" << i << "] Text => T_2[ " << j << "]"
                  << std::endl;
       }

       // Query => T_1[i]
       // Text => T_2[j]

       // perform matching
       match(eds_w.data[i].data, &q_suffix_trees[j], &candidate_matches, parameters);

       // filter matches
       filter_matches(candidate_matches, eds_q.str_slices[j],
                      eds_q.str_offsets[j], eds_w.str_offsets[i],
                      &j_active_suffixes, i, &valid_matches, parameters);

       txt_boundary = compute_letter_boundaries(eds_q.str_offsets[j]);
       qry_boundary = compute_letter_boundaries(eds_w.str_offsets[i]);

       // update edge
       create_edge(&g, valid_matches, 1, qry_boundary, txt_boundary, i, j,
                   parameters);

       // clean up
       clean_up();
    }
  }

  /*
   * Connect to start and accept nodes
   * ---------------------------------
   */

  txt_boundary = compute_letter_boundaries(eds_w.str_offsets[len_w - 1]); // w
  qry_boundary = compute_letter_boundaries(eds_q.str_offsets[len_q - 1]); // q

  int i = g.compute_index(txt_boundary.second + 1, qry_boundary.second +1, len_w - 1, len_q - 1);

  // std::cerr << txt_boundary.second << " " << qry_boundary.second << " i: " << i << std::endl;

  g.q_a.push_back(Edge{.dest = i, .weight = 0});


  return g;
}

/**
 *
 *
 *
 *
 */
int match_stats(Graph &g, EDS &eds_w, EDS &eds_q, core::Parameters const &parameters) {
  int max = INT_MIN;
  int letter = parameters.match_stats_letter_idx;

  if (parameters.match_stats_str != 1 && parameters.match_stats_str != 2) {
    std::cerr << " " << std::endl;
    exit(1);
  }

  if (parameters.match_stats_str == 1) {
    std::vector<int> start_nodes;
    int end_node = g.V - 1;
    int l = compute_letter_boundaries(eds_w.str_offsets[letter]).first;

    // get all possible start nodes O(N)
    for (int j = 0; j < eds_q.length; j++) {
      int k = compute_letter_boundaries(eds_q.str_offsets[j]).first;
      std::vector<span> o = eds_q.str_offsets[j];
      std::vector<int> ks;

      ks.push_back(k);

      start_nodes.push_back(g.compute_index(l, k, letter, j));

      for (int idx = 1; idx < o.size(); idx++) {
        ks.push_back(idx);
        start_nodes.push_back(g.compute_index(l, idx, letter, j));
      }
    }

    for (auto start_node : start_nodes) {
      int len = g.longest_frm_start(start_node);
      if (len > max) {
        max = len;
      }
    }
  }

  if (parameters.match_stats_str == 2) {
    std::vector<int> start_nodes;
    int end_node = g.V-1;
    int k = compute_letter_boundaries(eds_q.str_offsets[letter]).first;

    // get all possible start nodes O(N)
    for (int i = 0; i < eds_w.length; i++) {
      int l = compute_letter_boundaries(eds_w.str_offsets[i]).first;
      std::vector<span> o = eds_q.str_offsets[i];
      std::vector<int> ls;

      ls.push_back(k);

      start_nodes.push_back(g.compute_index(l, k, i, letter));

      for (int idx = 1; idx < o.size(); idx++) {
        ls.push_back(idx);
        start_nodes.push_back(g.compute_index(l, idx, i, letter));
      }
    }

    for (auto start_node : start_nodes) {
      int len = g.longest_frm_start(start_node);
      if (len > max) { max = len; }
    }
  }

    
  return max;
}

/**
 *
 *
 *
 *
 */
void foobar(Graph g, EDS &eds_w, EDS &eds_q) {
  int w_start, w_stop, q_start, q_stop;
  int l, l_prime, k, k_prime;

  l = compute_letter_boundaries(eds_w.str_offsets[w_start]).first;
  l_prime = compute_letter_boundaries(eds_w.str_offsets[w_stop]).second;

  k = compute_letter_boundaries(eds_q.str_offsets[w_start]).first;
  k_prime = compute_letter_boundaries(eds_q.str_offsets[w_stop]).second;

  int start_node = g.compute_index(l-1, k-1, w_start, q_start);
  int end_node = g.compute_index(l + 1, k + 1, w_stop, q_stop);
}

int longest_witness(Graph g) {
  return g.witness(0, g.V-1);
}

int shortest_witness(Graph g) {
  return g.djikstra(0, g.V - 1);
}

}
