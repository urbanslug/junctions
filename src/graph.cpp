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

#include "./parser.hpp"
#include "core.hpp"
#include "utils.hpp"

//using namespace std;

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

// TODO: move to a more central location
struct compare_by_weight {
  bool operator()(const pair<int, int> &l, const pair<int, int> &r) {
    return l.second > r.second;
  }

  bool operator()(const Edge &l, const Edge &r) { return l.weight > r.weight; }
};

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
    this->V = (N_1 + 1) * (N_2 + 1);
    // std::cerr << junctions::indent(2) << "v: " << V << junctions::N_2 << N_1 << junctions::N_2 << N_2;
    ///this->err = false;

    try {
      adj.resize(V);
    } catch (const std::bad_alloc &) {
      std::cerr << "not enough memory for this graph" << std::endl;
      exit(-1);
    } catch (const std::length_error  &) {
      std::cerr << "graph would be too large" << std::endl;
      exit(-1);
    }
  }

  // x in [1 - n1 -1], also l
  // y in [1 - n2 -1], also k
  // zero indexed
  int compute_index(int x, int y) {

    // return ((y+j)*N_1) + x+i;
    return (x * (N_2+1)) + y;
  }

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
    // TODO: remove
    // int stop_node = this->adj.size();

    auto compute_table_idx = [&start_node_idx](int gr_idx) -> int { return gr_idx - start_node_idx; };
    // TODO: remove
    // auto compute_graph_idx = [&start_node_idx](int tbl_idx) -> int { return tbl_idx + start_node_idx + 1; };
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

      if (visited.count(current_node) > 0) { continue; }

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

  /**
   * longest path from start_node_idx to stop_node_idx
   *
   *
   * @param[in] start_node_idx
   * @param[in] stop_node_idx
   * @return
   */
  int witness(int start_node_idx, int stop_node_idx) {
    auto tbl_idx = [&start_node_idx](int gr_idx) -> int { return gr_idx - start_node_idx; };
    // TODO: remove?
    // auto gr_idx = [&start_node_idx](int tbl_idx) -> int { return tbl_idx + start_node_idx + 1; };

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
  int dijkstra(int start_node_idx, int stop_node_idx) {

    // a vector of distances from start_node_idx to every other node
    std::vector<int> dists(this->V, INT_MAX);
    std::vector<bool> explored(this->V, false);

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, compare_by_weight> min_queue;

    // set distance from start_node_idx to itself as 0
    dists[start_node_idx] = 0;

    min_queue.push(std::make_pair(start_node_idx, dists[start_node_idx]));

    // a pair of node idx and weight
    std::pair<int, int> u;
    int i; // i is the node index w is the weight

    while (!min_queue.empty()) {
      u = min_queue.top();
      i = u.first;

      if (i == stop_node_idx) { return dists[i]; }

      explored[i] = true;
      min_queue.pop();

      for (auto ej : this->adj[i].outgoing) {

        // relax
        if ( dists[i] + ej.weight < dists[ej.dest] ) {
          dists[ej.dest] = dists[i] + ej.weight;
        }

        // add to min queue
        if (!explored[ej.dest]) {
          min_queue.push(std::make_pair(ej.dest, dists[ej.dest]) );
        }
      }
    }

    return dists[stop_node_idx];
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
               int weight,
               std::string str,
               core::Parameters const &parameters,
               int eps_side = 0
               ) {

    if (parameters.verbosity > 2) { std::cerr << utils::indent(1) << "DEBUG, [Graph::g.addEdge]" << std::endl; }

    int stop, start;
    int l, k, l_prime, k_prime;

    if (parameters.verbosity > 4) {
      std::cerr << junctions::indent(2) << "w_m => (" << i_boundary.first << ", " << i_boundary.second << ")" << std::endl
                << junctions::indent(2) << "q_m => (" << j_boundary.first << ", " << j_boundary.second << ")" << std::endl;
    }

    l = w_m.first == junctions::match_type::exp ? i_boundary.first : N_1;
    k = q_m.first == junctions::match_type::exp ? j_boundary.first : N_2;

    start = compute_index(l, k);

    l_prime = w_m.second == junctions::match_type::exp ? i_boundary.second + 1 : N_1 + weight;
    k_prime = q_m.second == junctions::match_type::exp ? j_boundary.second + 1 : N_2 + weight;

    if (eps_side == 1) { k_prime = k; }

    if (eps_side == 2) { l_prime = l; }

    stop = compute_index(l_prime, k_prime);

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
      std::cerr << utils::indent(2) << "(" + junctions::N_1 +", " + junctions::N_2 + ") (" << N_1 << ", " << N_2 << ")"
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

  void dbg_print(int indent_level=0) {
    std::cerr << utils::indent(indent_level) << "Graph info {" << std::endl
              << utils::indent(indent_level + 1) << "nodes in " << junctions::T_1 << ": " << this->N_1 << std::endl
              << utils::indent(indent_level + 1) << "nodes in " << junctions::T_2 << ": " << this->N_2 << std::endl
              << utils::indent(indent_level + 1) << "size of intersection graph: " << this->V << std::endl
              << utils::indent(indent_level) << "}" << std::endl;
  }

  void print_dot() {
    auto print_idx = [&](int idx) -> std::string {
      if (idx == 0) { return junctions::q_0; }
      if (idx == this->V - 1) { return junctions::q_a; }
      return std::to_string(idx);
    };

    auto print_node = [&](int idx, std::string color) {
      if (idx == 0 || idx == this->V - 1) { color = "green"; }
      std::cout << "\t" << print_idx(idx) << " [color=\"" << color << "\"];" << std::endl;
    };

    auto print_label = [](Edge e) -> std::string {
      if (e.is_eps) {
        return " \"(" + junctions::unicode_eps + ", " + std::to_string(e.weight) + ")\"";
      } else {
        return " \"(" + e.str + ", " + std::to_string(e.weight) + ")\"";
      }
    };

    std::cout << "digraph G {\n"
              << "\trankdir = LR;\n"
              << "\tnode[shape = circle];\n";

    // print nodes
    for (int i = 0; i < this->V; i++) {
      switch (this->adj[i].vertex_type) {
      case 0:
        print_node(i, "blue");
        break;
      case 1:
      case 2:
        print_node(i, "orange");
        break;
      case 3:
        print_node(i, "red");
        break;
      default:
        break;
      }
    }

    // print edges
    std::string s, label;
    Vertex v;

    for (int i=0; i < this->V; i++) {
      v = this->adj[i];

      // unreachable
      if (v.incoming.empty() && v.outgoing.empty()) {
        // if (false) { std::cout << "\t" + print_idx(i) + ";\n"; }
        continue;
      }

      for (auto e : v.outgoing) {
        std::cout << "\t" << print_idx(i) << " -> " << print_idx(e.dest)
                  << " [label=" << print_label(e)
                  << ", weight=" << std::to_string(e.weight)
                  << "];\n";
      }
    }

    std::cout << "}" << std::endl;
  }
};

// TODO: merge with utils
std::pair<int, int> compute_letter_boundaries(std::vector<span> const &offsets, bool has_eps = false) {
  int left = offsets.front().start;

  int right = has_eps ? offsets.back().stop : offsets.back().stop;

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
                    matrix *txt_active_suffixes,
                    matrix *qry_active_suffixes,
                    int qry_letter_idx,
                    int txt_letter_idx,
                    std::vector<junctions::graph_slice> *v,
                    core::Parameters const &parameters) {

  if (parameters.verbosity > 2) {
    std::cerr << utils::indent(1) << "DEBUG, [graph::filter_matches]"
              << std::endl;
  }


  //int txt_start_in_N = txt_offsets[0].start;
  auto in_txt_N = [&](int k) -> int { return txt_offsets[0].start + k; };
  auto in_txt_N_2 = [&](int idx, int k) -> int { return txt_offsets[idx].start + k; };

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
      std::cerr << utils::indent(2)
                << "txt_str_idx: " << candiate_match.text_str_index
                << " start char in concat l: " << candiate_match.text_char_index
                << " start char in l:  " << match_start_in_txt
                << " char start in N (not concat): " << t_start_in_N
                << std::endl;
    }

    // int txt_end = txt_slice.start + txt_slice.length;

    // the match starts within a string in the text
    // second condition because actv suff can only be extended ...
    if (match_start_in_txt > 0 && qry_letter_idx > 0) {
        // is valid active suffix
        // int in_N = in_txt_N(candiate_match.text_char_index); wrong
      valid_as = ((*txt_active_suffixes)[qry_letter_idx - 1][t_start_in_N] == 1);
      bool shorter_match{false};
      int pos;
      for (int idx{1}; idx < candiate_match.match_length; idx++) {
        if ((*txt_active_suffixes)[qry_letter_idx - 1][t_start_in_N + idx] == 1) {
          pos = idx;
          shorter_match = true;
          break;
        }
      }

      if (shorter_match) {
        t_start_in_N += pos;
        match_start_in_txt += pos;
        candiate_match.text_char_index += pos;
        candiate_match.match_length -= pos;
        candiate_match.str = candiate_match.str.substr(pos);
        valid_as = true;
      }
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

    // TODO we don't need this anymore, right? we get this from the ST correctly
    //int m_len= -1; // the actual length of the match

    // where the match ends
    // TODO: can it be zero? should we use a smaller value?
    // size_t is long unsigned int
    std::size_t candidate_match_end = match_start_in_txt + candiate_match.match_length;

    // where the matched string actually ends
    std::size_t txt_slice_end = txt_slice.start + txt_slice.length;

    if (parameters.verbosity > 3) {
      std::cerr << utils::indent(2)
                << "txt_slice.start " << txt_slice.start
                << " candidate_match_end " << candidate_match_end
                << " txt_slice_end " << txt_slice_end
                << std::endl;
    }

    // find where the match ends within the txt string
    if (txt_slice.start + candidate_match_end >= txt_slice_end) {
      // the match went beyond the txt so we limit it to the end
      // a match that is beyond the text also lies here

      match_end = txt_slice_end; // actual idx is -1

      //m_len = match_end - (txt_slice.start + match_start_in_txt);
      t_m_stop = junctions::match_type::exp;
    } else {
      // within the text
      match_end = candidate_match_end; // actual idx is -1
      //m_len = candiate_match.match_length;

      t_m_stop = junctions::match_type::imp;

      // handle active suffix
      // this creates and active suffix
      // set match end as an active suffix
      int in_N = in_txt_N_2(candiate_match.text_str_index, match_end);

      // std::cerr << utils::indent(2) << "save as: " << qry_letter_idx << in_N;
      (*txt_active_suffixes)[qry_letter_idx][in_N] = 1;
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

    // create an active suffix in the query
    if (candiate_match.beyond_txt) {
      (*qry_active_suffixes)[txt_letter_idx][q_start_in_N + candiate_match.match_length] = 1;
    }

    if (parameters.verbosity > 3) {
      std::cerr << utils::indent(2)
                << "beyond txt: " << candiate_match.beyond_txt
                << " (txt): " << t_m_start << ", " << t_m_stop
                << " (qry) " << q_m_start << ", " << q_m_stop
                << std::endl;
    }

    junctions::graph_slice s =
        junctions::graph_slice{.txt_start = t_start_in_N,
                               .qry_start = q_start_in_N,
                               .q_m = std::make_pair(q_m_start, q_m_stop),
                               .t_m = std::make_pair(t_m_start, t_m_stop),
                               .len = candiate_match.match_length,
                               .str = candiate_match.str};

    if (parameters.verbosity > 3) {
      s.dbg_print(2); }

      v->push_back(s);
  }

}

/**
 *
 * @param[in] g
 * @param[in] valid_matches
 *
 */
void create_edge(Graph *g,
                 std::vector<junctions::graph_slice> const &valid_matches,
                 int qry,
                 std::pair<int, int> qry_boundary,
                 std::pair<int, int> txt_boundary,
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
                 x.q_m, x.t_m, x.len, x.str, parameters, eps_side);
    }
    break;
  case 2:
    // T_2 is the qry
    if (eps_edge) { eps_side = 1; }
    for (auto x : valid_matches) {
      g->addEdge(x.txt_start, x.qry_start, txt_boundary, qry_boundary, x.t_m,
                 x.q_m, x.len, x.str, parameters, eps_side);
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
Graph compute_intersection_graph(EDS &eds_w,
                                 EDS &eds_q,
                                 core::Parameters const &parameters) {
  if (parameters.verbosity > 1) { std::cerr << "DEBUG [graph::compute_intersection_graph]" << std::endl; }
  if (parameters.verbosity > 0) { junctions::print_eds_info(eds_w, eds_q); }

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

  gen_suffix_tree(eds_w, &w_suffix_trees);
  gen_suffix_tree(eds_q, &q_suffix_trees);

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

  Graph g = Graph(eds_w.size + eds_w.epsilons, eds_q.size + eds_q.epsilons);

  if (parameters.verbosity > 0) { g.dbg_print(); }

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

        if (parameters.verbosity > 4) {
          std::cerr << utils::indent(1)
                    << "DEBUG [graph::compute_intersection_graph] "
                    << junctions::unicode_eps << " at "
                    << junctions::T_2 << "[" << j << "]" << std::endl;
        }

        int eps_idx = eds_q.str_offsets[j].back().start;

        txt_boundary = compute_letter_boundaries(eds_q.str_offsets[j], true);
        qry_boundary = compute_letter_boundaries(eds_w.str_offsets[i]);

        // std::cerr << junctions::indent(2) << "txt_b => (" << txt_boundary.first << ", " << txt_boundary.second << ")" << std::endl
        //           << junctions::indent(2) << "qry_b => (" << qry_boundary.first << ", " << qry_boundary.second << ")" << std::endl;

        valid_matches.push_back(junctions::graph_slice{
            .txt_start = eps_idx,
            .qry_start = qry_boundary.first,
            .q_m = std::make_pair(junctions::match_type::exp, junctions::match_type::exp),
            .t_m = std::make_pair(junctions::match_type::exp, junctions::match_type::exp),
            .len = 0,
            .str = ""});

        // std::cerr << "b->" << qry_boundary.first << "\n";

        // the last letter in T_1
        if (i == len_w - 1) {
          std::pair<int, int>  accept_qry_boundary = std::make_pair(qry_boundary.second + 1, qry_boundary.second + 1);
          // std::cerr << "------ "<< qry_boundary.second << std::endl;
          create_edge(&g, valid_matches, 1, accept_qry_boundary, txt_boundary, parameters, true);
        }

        // handle active suffixes
        if (j > 0) {
          for (std::size_t col=0; col <= eds_w.str_offsets[i].back().stop; col++) {
            if (i_active_suffixes[j-1][col]) {
              // update active suffixes
              i_active_suffixes[j][col] = 1;

              // update active suffixes
              valid_matches.push_back(junctions::graph_slice{
                  .txt_start = eps_idx,
                  .qry_start = static_cast<int>(col),
                  .q_m = std::make_pair(junctions::match_type::imp, junctions::match_type::imp),
                  .t_m = std::make_pair(junctions::match_type::exp, junctions::match_type::exp),
                  .len = 0,
                  .str = ""});
            }
          }
        }

        create_edge(&g, valid_matches, 1, qry_boundary, txt_boundary, parameters, true);

      }


      clean_up();

      // T_1[i] has epsilon
      // T_2[j] is the query
      if (eds_w.data[i].has_epsilon) {

        if (parameters.verbosity > 4) {
          std::cerr << utils::indent(1)
                    << "DEBUG [graph::compute_intersection_graph] "
                    << junctions::unicode_eps << " at "
                    << junctions::T_1 << "[" << i << "]" << std::endl;
        }

        txt_boundary = compute_letter_boundaries(eds_w.str_offsets[i], true);
        qry_boundary = compute_letter_boundaries(eds_q.str_offsets[j]);

        int eps_idx = eds_w.str_offsets[i].back().start;

        valid_matches.push_back(junctions::graph_slice{
            .txt_start = eps_idx,
            .qry_start = qry_boundary.first,
            .q_m = std::make_pair(junctions::match_type::exp, junctions::match_type::exp),
            .t_m = std::make_pair(junctions::match_type::exp, junctions::match_type::exp),
            .len = 0,
            .str = ""});



        // the last letter in T_2
        if (j == len_q - 1) {
          std::pair<int, int> accept_qry_boundary =
              std::make_pair(qry_boundary.second + 1, qry_boundary.second + 1);
          // std::cerr << "------ "<< qry_boundary.second << std::endl;
          create_edge(&g, valid_matches, 2, accept_qry_boundary, txt_boundary, parameters, true);
        }


        // update active suffixes
        if (i > 0) {
          for (std::size_t col = 0; col <= eds_q.str_offsets[j].back().stop;
               col++) {
            if (j_active_suffixes[i - 1][col]) {
              // update active suffixes
              j_active_suffixes[i][col] = 1;

              // update active suffixes
              valid_matches.push_back(junctions::graph_slice{
                  .txt_start = eps_idx,
                  .qry_start = static_cast<int>(col),
                  .q_m = std::make_pair(junctions::match_type::imp, junctions::match_type::imp),
                  .t_m = std::make_pair(junctions::match_type::exp, junctions::match_type::exp),
                  .len = 0,
                  .str = ""});
            }
          }
        }

        create_edge(&g, valid_matches, 2, qry_boundary, txt_boundary, parameters, true);
      }

      clean_up();

      // Search for j_strs in T_1[i]
      // ---------------------------

      // Query => T_2[j]
      // Text => T_1[i]

      if (parameters.verbosity > 3) {
        std::cerr << utils::indent(1)
                  << "Query => T" << junctions::unicode_sub_2 << "[" << j << "] "
                  << "Text => T" << junctions::unicode_sub_1 << "[" << i << "]"
                  << std::endl;
      }

      // perform matching
      junctions::perform_matching(eds_q.data[j].data, eds_w.str_slices[i],
                                 &w_suffix_trees[i], &candidate_matches,
                                 parameters);

      // filter matches
      filter_matches(candidate_matches, eds_w.str_slices[i],
                     eds_w.str_offsets[i], eds_q.str_offsets[j],
                     &i_active_suffixes, &j_active_suffixes,
                     j, i, &valid_matches, parameters);

      qry_boundary = compute_letter_boundaries(eds_q.str_offsets[j]);
      txt_boundary = compute_letter_boundaries(eds_w.str_offsets[i]);

      // update edge
       create_edge(&g, valid_matches, 2, qry_boundary, txt_boundary, parameters);

       // clean up
       clean_up();

       // Search for i_strs in T_2[j]
       // ---------------------------

       if (parameters.verbosity > 3) {

        std::cerr << utils::indent(1)
                  << "Query => T" << junctions::unicode_sub_1 << "[" << i << "] "
                  << "Text => T" << junctions::unicode_sub_2 << "[" << j << "]"
                  << std::endl;
       }

       // Query => T_1[i]
       // Text => T_2[j]

       // perform matching
       junctions::perform_matching(eds_w.data[i].data, eds_q.str_slices[j],
                                  &q_suffix_trees[j], &candidate_matches,
                                  parameters);

       // filter matches
       filter_matches(candidate_matches, eds_q.str_slices[j],
                      eds_q.str_offsets[j], eds_w.str_offsets[i],
                      &j_active_suffixes, &i_active_suffixes,
                      i, j, &valid_matches, parameters);

       txt_boundary = compute_letter_boundaries(eds_q.str_offsets[j]);
       qry_boundary = compute_letter_boundaries(eds_w.str_offsets[i]);

       // update edge
       create_edge(&g, valid_matches, 1, qry_boundary, txt_boundary, parameters);

       // clean up
       clean_up();
    }
  }

  /*
   * Connect to start and accept nodes
   * ---------------------------------
   */

  // TODO: remove this capping stuff

  // txt_boundary = compute_letter_boundaries(eds_w.str_offsets[len_w - 1]); // w
  // qry_boundary = compute_letter_boundaries(eds_q.str_offsets[len_q - 1]); // q

  // int a = txt_boundary.second + 1;
  // int b = qry_boundary.second + 1;

  // int i = g.compute_index(a, b);

  // std::cerr << "-------> capping\n";
  // std::cerr << a << " " << b << " i: " << i << std::endl;

  // g.q_a.push_back(Edge{.dest = i, .weight = 0});


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

    int l = compute_letter_boundaries(eds_w.str_offsets[letter]).first;

    // get all possible start nodes O(N)
    for (std::size_t j = 0; j < eds_q.length; j++) {
      int k = compute_letter_boundaries(eds_q.str_offsets[j]).first;
      std::vector<span> o = eds_q.str_offsets[j];
      std::vector<int> ks;

      ks.push_back(k);

      start_nodes.push_back(g.compute_index(l, k));

      for (std::size_t idx = 1; idx < o.size(); idx++) {
        ks.push_back(idx);
        start_nodes.push_back(g.compute_index(l, idx));
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

    int k = compute_letter_boundaries(eds_q.str_offsets[letter]).first;

    // get all possible start nodes O(N)
    for (std::size_t i = 0; i < eds_w.length; i++) {
      int l = compute_letter_boundaries(eds_w.str_offsets[i]).first;
      std::vector<span> o = eds_q.str_offsets[i];
      std::vector<int> ls;

      ls.push_back(k);

      start_nodes.push_back(g.compute_index(l, k));

      for (std::size_t idx = 1; idx < o.size(); idx++) {
        ls.push_back(idx);
        start_nodes.push_back(g.compute_index(l, idx));
      }
    }

    for (auto start_node : start_nodes) {
      int len = g.longest_frm_start(start_node);
      if (len > max) { max = len; }
    }
  }


  return max;
}

int longest_witness(Graph g) {
  return g.witness(0, g.V-1);
}

int shortest_witness(Graph g) {
  return g.dijkstra(0, g.V - 1);
}

}
