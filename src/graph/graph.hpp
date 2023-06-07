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
 */
enum match_type { exp, imp };

class MatchTypePair{
  match_type l;
  match_type r;
public:
  MatchTypePair(match_type l, match_type r) : l(l), r(r) {}
  match_type left() const {return this->l;}
  match_type right() const {return this->r;}

  bool is_imp_imp() const {
    return l == match_type::imp && r == match_type::imp;
  }

  bool is_imp_exp() const {
    return l == match_type::imp && r == match_type::exp;
  }

  bool is_exp_imp() const {
    return l == match_type::exp && r == match_type::imp;
  }

  bool is_exp_ext() const {
    return l == match_type::exp && r == match_type::exp;
  }

  std::string to_string() const {
    return std::string(l == match_type::exp ? "exp" : "imp") + "-" + std::string(r == match_type::exp ? "exp" : "imp");
  }
};

class GraphSlice {
  std::size_t txt_start; // text start in N
  std::size_t qry_start; // query start in N

  MatchTypePair qry_match_typ; // match type of the qry
  MatchTypePair txt_match_typ; // match type of the txt

  std::size_t match_length;
  std::string str;

public:
  // TODO: Constructor with initialization list
  GraphSlice(std::size_t txt_start,
             std::size_t qry_start,
             MatchTypePair qry_match_typ,
             MatchTypePair txt_match_typ,
             std::size_t match_length,
             std::string str)
    : txt_start(txt_start),
      qry_start(qry_start),
      qry_match_typ(qry_match_typ),
      txt_match_typ(txt_match_typ),
      match_length(match_length),
      str(str) {}

    GraphSlice(std::size_t txt_start,
             std::size_t qry_start,
             MatchTypePair qry_match_typ,
             MatchTypePair txt_match_typ)
    : txt_start(txt_start),
      qry_start(qry_start),
      qry_match_typ(qry_match_typ),
      txt_match_typ(txt_match_typ),
      match_length(0),
      str(std::string{}) {}

    std::size_t get_txt_start() const { return this->txt_start; }
    std::size_t get_qry_start() const { return this->qry_start; }
    MatchTypePair get_qry_match_typ() const { return this->qry_match_typ; }
    MatchTypePair get_txt_match_typ() const { return this->txt_match_typ; }
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
  int vertex_type;

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


  bool is_exp_exp(int idx);

  bool is_exp_imp(int idx);

  bool is_imp_exp(int idx);

  bool is_imp_imp(int idx);

public:

  std::size_t get_size();

  std::size_t compute_index(std::size_t x, std::size_t y);

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
                   graph::MatchTypePair w_m,
                   graph::MatchTypePair q_m,
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

/**
 *
 *
 *
 *
 */
int match_stats(Graph &g,
                eds::EDS &eds_w,
                eds::EDS &eds_q,
                core::Parameters const &parameters);

std::size_t multiset(graph::Graph &g);

int longest_witness(Graph& g);

int shortest_witness(Graph& g);
} // namespace graph
#endif
