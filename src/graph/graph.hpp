#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <climits>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <ostream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>
#include <set>


#include "../eds/eds.hpp"
#include "../core/core.hpp"


namespace graph {
/**
 * @brief Is a match explicit or implicit
 * explicit: a match start (or ends) from the start (or end) of a string
 * implicit: a match starts (or ends) within a string
 * n: a match starts (or ends) within a string
 */
enum match_type { exp, imp, n };

/**
 * 0 exp exp
 * 1 exp imp
 * 2 imp exp
 * 3 imp imp
 */
enum match_type_pair {
  exp_exp,
  exp_imp,
  imp_exp,
  imp_imp,
  inv // invalid
};

class MatchTypePairUnion {
  match_type l1;
  match_type r1;

  match_type l2;
  match_type r2;

  /*
    maps a match type pair to an int between 0 and 3

    0 exp exp
    1 exp imp
    2 imp exp
    3 imp imp
  */
  constexpr static int to_integral(match_type lhs, match_type rhs) {
    return int(lhs) * int(match_type::n) + int(rhs);
  }

  match_type_pair get_val(match_type v1, match_type v2) {
    switch (to_integral(v1, v2)) {
    case (to_integral(exp, exp)):
      return exp_exp;
      break;
    case (to_integral(imp, exp)):
      return exp_imp;
      break;
    case (to_integral(exp, imp)):
      return imp_exp;
      break;
    default:
      return imp_imp;
    }
  }

public:
  MatchTypePairUnion(match_type l1, match_type r1, match_type l2, match_type r2)
      : l1(l1), r1(r1), l2(l2), r2(r2) {}

  match_type_pair get_val_left() { return get_val(this->l1, this->l2); }
  match_type_pair get_val_right() { return get_val(this->r1, this->r2); }
  match_type left1() const {return this->l1;}
  match_type right1() const {return this->r1;}
  match_type left2() const {return this->l2;}
  match_type right2() const {return this->r2;}
};

class GraphSlice {
  std::size_t txt_start; // text start in N
  std::size_t qry_start; // query start in N

  MatchTypePairUnion m_typ;

  std::size_t match_length;
  std::string str;

public:
  // TODO: Constructor with initialization list
  GraphSlice(std::size_t txt_start,
             std::size_t qry_start,
             MatchTypePairUnion m_typ,
             std::size_t match_length,
             std::string str)
    : txt_start(txt_start),
      qry_start(qry_start),
      m_typ(m_typ),
      match_length(match_length),
      str(str) {}

    GraphSlice(std::size_t txt_start,
               std::size_t qry_start,
               MatchTypePairUnion m_typ)
    : txt_start(txt_start),
      qry_start(qry_start),
      m_typ(m_typ),
      match_length(0),
      str(std::string{}) {}

    std::size_t get_txt_start() const { return this->txt_start; }
    std::size_t get_qry_start() const { return this->qry_start; }
    MatchTypePairUnion get_match_typ() const { return this->m_typ; }
    std::size_t get_match_length() const { return this->match_length; }
    std::string const& get_str() { return this->str; }
};

/**
 * edge struct to represent a weighted edge
 */
struct Edge {
  std::size_t dest;   // destination vertex
  std::size_t weight; // weight of the edge, length of a string in characters.
  std::string str;
  bool is_eps; // is the edge an epsilon/hybrid edge

  Edge(std::size_t d, std::size_t w, std::string s, bool b);
};
bool operator==(const Edge &lhs, const Edge &rhs);
  //bool operator<(const Edge &lhs, const Edge &rhs);
bool operator<(Edge const &lhs, Edge const &rhs);
std::ostream &operator<<(std::ostream &os, const Edge &e);

// TODO: move to a more central location
struct compare_by_weight {
  bool operator()(const std::pair<std::size_t, std::size_t> &l,
                  const std::pair<std::size_t, std::size_t> &r);

  bool operator()(const graph::Edge &l, const graph::Edge &r);
};

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
  // use an enum?
  match_type_pair vertex_type;

  // initialize a struct with default value
  Vertex();
};

/**
 *
 *
 *
 * adj adjacency list to store edges
 */
class Graph {
  std::size_t V;                    // number of vertices
  std::size_t N_1;                  // yyy
  std::size_t N_2;                  // xxx
  // std::set<int> q_0; // start node // TODO: remove
  std::vector<Edge> q_a; // accept node
  std::vector<Vertex> adj; // adjacency list to store edges

  std::vector<std::size_t> match_stats; // match stats


  Vertex const& get_node(std::size_t node_idx);

public:

  std::size_t get_size();

  std::size_t last_node() const;

  std::size_t compute_index(std::size_t x, std::size_t y);

  void compute_match_stats();

  std::size_t get_match_stats(std::size_t node_idx);

  // compute the size of the multiset
  std::size_t multiset_size();

  /**
   * what is the furthest path from this given start node
   *
   *
   * @param[in] start_node_idx
   * @return
   */
  std::size_t longest_frm_start(std::size_t start_node_idx);

  /**
   * longest path from start_node_idx to stop_node_idx
   *
   *
   * @param[in] start_node_idx
   * @param[in] stop_node_idx
   * @return
   */
  int witness(std::size_t start_node_idx, std::size_t stop_node_idx);

  /**
   * Using dijkstra with a min heap to compute the
   * shortest path from start_node_idx to stop_node_idx
   *
   *
   *
   * @param[in] start_node_idx
   * @param[in] stop_node_idx
   * @return
   */
  int dijkstra(std::size_t start_node_idx, std::size_t stop_node_idx);


  void create_edge(std::vector<GraphSlice> const &valid_matches,
                   int qry,
                   std::pair<std::size_t, std::size_t> qry_boundary,
                   std::pair<std::size_t, std::size_t> txt_boundary,
                   bool eps_edge);

      // function to add an edge to the graph
      // src is T_1
      // src is T_2
      void add_edge(std::size_t N_1, std::size_t N_2,
                   std::pair<std::size_t, std::size_t> i_boundary,
                   std::pair<std::size_t, std::size_t> j_boundary,
                   graph::MatchTypePairUnion m_typ,
                   std::size_t weight, std::string str,
                   int eps_side);

  // constructor to initialize the graph
  Graph(std::size_t N_1, std::size_t N_2);

  void dbg_print(int indent_level);

  void print_dot();
};

/**
*
*
* @param[in] eds_w
* @param[in] eds_q
* @param[in] parameters
* return
*/
Graph compute_intersection_graph(eds::EDS &eds_w,
                                 eds::EDS &eds_q,
                                 core::Parameters const &parameters);

std::size_t match_stats(Graph &g,
                        std::size_t letter_start,
                        std::size_t last,
                        core::ed_string match_stats_str);

std::size_t multiset(graph::Graph &g);

int longest_witness(Graph& g);

int shortest_witness(Graph& g);
} // namespace graph
#endif
