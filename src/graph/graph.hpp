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

//using namespace std;

namespace graph {
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
                  const std::pair<std::size_t, std::size_t> &r) {
    return l.second > r.second;
  }

  bool operator()(const graph::Edge &l, const graph::Edge &r) {
    return l.weight > r.weight;
  }
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
  std::size_t witness(std::size_t start_node_idx, std::size_t stop_node_idx);

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
  std::size_t dijkstra(std::size_t start_node_idx, std::size_t stop_node_idx);


  void create_edge(std::vector<n_junctions::graph_slice> const &valid_matches,
                   int qry,
                   std::pair<std::size_t, std::size_t> qry_boundary,
                   std::pair<std::size_t, std::size_t> txt_boundary,
                   n_core::Parameters const &parameters,
                   bool eps_edge);

      // function to add an edge to the graph
      // src is T_1
      // src is T_2
      void add_edge(std::size_t N_1, std::size_t N_2,
                   std::pair<std::size_t, std::size_t> i_boundary,
                   std::pair<std::size_t, std::size_t> j_boundary,
                   std::pair<n_junctions::match_type, n_junctions::match_type> w_m,
                   std::pair<n_junctions::match_type, n_junctions::match_type> q_m,
                   std::size_t weight, std::string str,
                   n_core::Parameters const &parameters, int eps_side);

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
Graph compute_intersection_graph(eds::EDS &eds_w, eds::EDS &eds_q,
                                 n_core::Parameters const &parameters);


/**
 *
 *
 *
 *
 */
int match_stats(Graph &g, eds::EDS &eds_w, eds::EDS &eds_q,
                n_core::Parameters const &parameters);

int longest_witness(Graph g);

int shortest_witness(Graph g);
} // namespace graph
#endif
