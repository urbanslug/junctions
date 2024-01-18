#include <climits>
#include <cmath>
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
#include <valarray>
#include <vector>
#include <queue>
#include <cmath>
#include <numeric>

#include "./graph.hpp"


// initialize a struct with default values
graph::Edge::Edge(std::size_t d, std::size_t w, std::string s, bool b)
  : dest(d), weight(w), str(s), is_eps(b) {}

bool graph::operator==(const graph::Edge &lhs, const graph::Edge &rhs) {
  return std::tie(lhs.dest, lhs.weight, lhs.str, lhs.is_eps) ==
         std::tie(rhs.dest, rhs.weight, rhs.str, rhs.is_eps);
}

bool graph::operator<(graph::Edge const &lhs, graph::Edge const &rhs) {
  return std::tie(lhs.dest, lhs.weight, lhs.str, lhs.is_eps) <
         std::tie(rhs.dest, rhs.weight, rhs.str, rhs.is_eps);
}

std::ostream &graph::operator<<(std::ostream &os, const graph::Edge &e) {
  os << "Edge {"
     << "dest: " << e.dest << " qry start: " << e.str << "len: " << e.weight
     << " str: " << e.is_eps << "}" << std::endl;
  return os;
}

bool graph::compare_by_weight::operator()(const std::pair<std::size_t, std::size_t> &l,
                const std::pair<std::size_t, std::size_t> &r) {
  return l.second > r.second;
}

bool graph::compare_by_weight::operator()(const graph::Edge &l,
                                          const graph::Edge &r) {
  return l.weight > r.weight;
}

// impl == for MatchTypePairUnion
bool graph::operator==(const graph::MatchTypePairUnion &lhs,
                       const graph::MatchTypePairUnion &rhs) {
  return
    lhs.left1() == rhs.left1() &&
    lhs.left2() == rhs.left2() &&
    lhs.right1() == rhs.right1() &&
    lhs.right2() == rhs.right2();
}

// initialize a struct with default values
graph::Vertex::Vertex()
  : incoming(std::set<Edge>{}),
    outgoing(std::set<Edge>{}),
    vertex_type(graph::match_type_pair::inv) {}

// -------
// classes
// -------

// constructor to initialize the graph
graph::Graph::Graph(std::size_t N_1, std::size_t N_2) {
  this->N_1 = N_1;
  this->N_2 = N_2;
  this->V = (N_1 + 1) * (N_2 + 1);
}

// x in [1 - n1 -1], also l
// y in [1 - n2 -1], also k
// zero indexed
std::size_t graph::Graph::compute_index(std::size_t x, std::size_t y) {
  return (x * (this->N_2 + 1)) + y;
}

graph::Vertex & graph::Graph::get_node(std::size_t node_idx) {
  std::map<size_t, graph::Vertex>::iterator it = sparse_adj.find(node_idx);
  if (it != sparse_adj.end()) {
    return it->second;
  }
  else {
    graph::Vertex v;
    sparse_adj.insert(std::make_pair(node_idx,v));
    return sparse_adj[node_idx];
  }
}

bool graph::Graph::check_match_stats(std::size_t node_idx) {
  return (this->match_stats.find(node_idx)!=match_stats.end());
}

std::size_t graph::Graph::get_match_stats(std::size_t node_idx) {
  return this->match_stats[node_idx];
}

std::map<std::size_t,size_t> const& graph::Graph::get_match_stats_vec() const {
  return this->match_stats;
}

std::size_t graph::Graph::get_size() { return this->V; }

std::size_t graph::Graph::last_node() const { return this->V-1; }


// TODO: replace i & j boundary with a struct
/**
 * function to add an edge to the graph
 */
void graph::Graph::add_edge(std::size_t N_1,
                            std::size_t N_2,
                            graph::BoundaryUnion bounds,
                            MatchTypePairUnion m_typ,
                            std::size_t weight,
                            std::string str,
                            int eps_side = 0) {
  int stop, start;
  int l, k, l_prime, k_prime;

  l = m_typ.left1() == graph::match_type::exp ? bounds.left1() : N_1;
  k = m_typ.left2() == graph::match_type::exp ? bounds.left2() : N_2;

  start = compute_index(l, k);

  l_prime = m_typ.right1() == graph::match_type::exp ? bounds.right1() + 1 : N_1 + weight;
  k_prime = m_typ.right2() == graph::match_type::exp ? bounds.right2() + 1 : N_2 + weight;

  if (eps_side == 1) { k_prime = k; }

  if (eps_side == 2) { l_prime = l; }

  stop = compute_index(l_prime, k_prime);


  bool is_eps = (eps_side == 1 || eps_side == 2) ? true : false;

  if (is_eps ) { weight = 0; str = "";}

  graph::Edge e_rev = graph::Edge(start, weight, str, is_eps);
  graph::Edge e = graph::Edge(stop, weight, str, is_eps);

    //graph::Vertex start_ver=this->get_node(start);
    //graph::Vertex stop_ver=this->get_node(stop);

  this->get_node(start).meta = std::to_string(l) + "," + std::to_string(k);
  this->get_node(stop).meta = std::to_string(l_prime) + "," + std::to_string(k_prime);

  this->get_node(start).outgoing.insert(e);
  this->get_node(stop).incoming.insert(e_rev);

  // start type, end type
  this->get_node(start).vertex_type = m_typ.get_val_left();
  this->get_node(stop).vertex_type = m_typ.get_val_right();
}

std::size_t graph::Graph::multiset_size() {
 //std::vector<std::size_t> dp_table(this->V, 0);
    std::map<size_t,size_t> dp_map;

  std::size_t start_node, stop_node;
  start_node = 0;
  stop_node = this->V - 1;
  //dp_table[start_node] = 1;
  dp_map[start_node] = 1;

  std::set<Edge> out;

  std::size_t current_node = start_node;
  std::queue<std::size_t> to_visit;
  std::set<std::size_t> queued;

  to_visit.push(current_node);

  while (!to_visit.empty()) {
    current_node = to_visit.front();
    //Vertex current_ver = this->get_node(current_node);
    to_visit.pop();


    out = this->get_node(current_node).outgoing;
    for (auto e : out) {
      dp_map[e.dest] += dp_map[current_node];
      if (!queued.count(e.dest)) {
        to_visit.push(e.dest);
        queued.insert(e.dest);
      }
    }
  }

  return dp_map[stop_node];
}

void graph::Graph::compute_match_stats() {
  std::map<size_t,size_t> dp_map;
  std::size_t max_k = 0;

  // Note that this expects that idx will overflow when we attempt to go below
  // zero therefore assumes the last node is not max size_t value
  for(std::map<size_t,Vertex>::reverse_iterator riter=sparse_adj.rbegin();riter!=sparse_adj.rend();++riter){
    for (auto e: riter->second.outgoing) {
      if (max_k < (e.weight + dp_map[e.dest]) ) {
        max_k = e.weight + dp_map[e.dest];
      }
    }
    if(dp_map.find(riter->first)==dp_map.end()){
        dp_map.insert(std::make_pair(riter->first,0));
    }
    dp_map[riter->first] += max_k;
    max_k = 0;
  }

  this->match_stats = dp_map;
}


/**
 * longest path from start_node_idx to stop_node_idx
 *
 *
 * @param[in] start_node_idx
 * @param[in] stop_node_idx
 * @return
 */
int graph::Graph::witness(std::size_t start_node_idx, std::size_t stop_node_idx) {
  auto tbl_idx = [&start_node_idx](std::size_t gr_idx) -> std::size_t { return gr_idx - start_node_idx; };
  // TODO: remove?
  // auto gr_idx = [&start_node_idx](int tbl_idx) -> int { return tbl_idx + start_node_idx + 1; };

  // int size = stop_node_idx - start_node_idx + 1;

 // std::vector<int> dists(this->V, INT_MIN);
  std::map<size_t,int> dists_map;

  std::stack<std::size_t> to_visit;
  std::set<std::size_t> visited;
  std::set<Edge> out;
  int current_dist{};
  bool end_reached{false};

  std::size_t current_node = start_node_idx;
  dists_map[current_node] = 0;
  to_visit.push(current_node);


  while (!to_visit.empty()) {
    current_node = to_visit.top();
    //Vertex current_ver=this->get_node(current_node);
    to_visit.pop();
    out = this->get_node(current_node).outgoing;

    if (visited.count(current_node) > 0) { continue; }

    current_dist = dists_map[current_node];

    for (auto e : out) {

      if (dists_map[tbl_idx(e.dest)] < current_dist + static_cast<int>(e.weight)) {
        if (e.dest == stop_node_idx) { end_reached = true; }
        dists_map[tbl_idx(e.dest)] = current_dist + static_cast<int>(e.weight);
      }

      to_visit.push(e.dest);
    }
    visited.insert(current_node);
  }

  if (end_reached) {
    return dists_map[tbl_idx(stop_node_idx)];
  } else {
    return -1;
  }
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
int graph::Graph::dijkstra(std::size_t start_node_idx, std::size_t stop_node_idx) {

  // a vector of distances from start_node_idx to every other node
  //std::vector<int> dists(this->V, INT_MAX);
  //std::vector<bool> explored(this->V, false);
  std::map<size_t,int> dists_map;
  std::map<size_t,bool> explored_map;

  bool end_reached{false};

  std::priority_queue<std::pair<std::size_t, std::size_t>, std::vector<std::pair<std::size_t, std::size_t>>,
                      compare_by_weight>
      min_queue;

  // set distance from start_node_idx to itself as 0
  dists_map[start_node_idx] = 0;

  min_queue.push(std::make_pair(start_node_idx, dists_map[start_node_idx]));

  // a pair of node idx and weight
  std::pair<std::size_t, std::size_t> u;
  std::size_t i; // i is the node index w is the weight

  while (!min_queue.empty()) {
    u = min_queue.top();
    i = u.first;
    //Vertex curr_ver=this->get_node(i);

    if (i == stop_node_idx) {
      return dists_map[i];
    }

    explored_map[i] = true;
    min_queue.pop();

    for (auto ej : get_node(i).outgoing) {

      // relax
      if((dists_map.find(ej.dest)==dists_map.end())|| (dists_map[i] + static_cast<int>(ej.weight) < dists_map[ej.dest])) {
        if (ej.dest == stop_node_idx) { end_reached = true; }
        dists_map[ej.dest] = dists_map[i] + ej.weight;
      }

      // add to min queue
      if ((explored_map.find(ej.dest)==explored_map.end()) || (!explored_map[ej.dest])) {
        min_queue.push(std::make_pair(ej.dest, dists_map[ej.dest]));
      }
    }
  }

  if (end_reached) {
    return dists_map[stop_node_idx];
  } else {
    return -1;
  }
}

void graph::Graph::dbg_print(int indent_level) {
  std::cerr << core::indent(indent_level)
            << "Graph info {" << std::endl
            << core::indent(indent_level + 1) << "nodes in " << core::constants::T_1
            << ": " << this->N_1 << std::endl
            << core::indent(indent_level + 1) << "nodes in " << core::constants::T_2
            << ": " << this->N_2 << std::endl
            << core::indent(indent_level + 1)
            << "size of uncompacted intersection graph: " << this->V << std::endl
            << core::indent(indent_level) << "}" << std::endl;
}

void graph::Graph::print_dot() {
  auto print_idx = [&](std::size_t idx) -> std::string {
    if (idx == 0) { return core::constants::q_0; }
    if (idx == this->V - 1) { return core::constants::q_a; }
    return std::to_string(idx);
  };


  auto print_node_label = [&](std::size_t idx) -> std::string {
    if (idx == 0) { return core::constants::q_0; }
    if (idx == this->V - 1) { return core::constants::q_a; }
    //Vertex ver=this->get_node(idx);
    return std::to_string(idx) + "\\n" + "(" + this->get_node(idx).meta + ")";
  };

  auto print_node = [&](std::size_t idx, std::string color) {
    if (idx == 0 || idx == this->V - 1) {
      color = "green";
    }
    std::cout << "\t" << print_idx(idx)
              << " [color=\"" << color << "\" label=\"" << print_node_label(idx) << "\"];"
              << std::endl;
  };

  auto print_label = [](Edge e) -> std::string {
    if (e.is_eps) {
      return " \"(" + core::constants::unicode_eps + ", " + std::to_string(e.weight) +
             ")\"";
    } else {
      return " \"(" + e.str + ", " + std::to_string(e.weight) + ")\"";
    }
  };

  std::cout << "digraph G {\n"
            << "\trankdir = LR;\n"
            << "\tnode[shape = circle];\n";

  // print nodes
  //for (std::size_t i{}; i < this->V; i++) {
  for(std::map<size_t,Vertex>::iterator iter=sparse_adj.begin();iter!=sparse_adj.end();++iter){
    //std::cout<< iter->second.vertex_type<<std::endl;
    switch (iter->second.vertex_type) {
    case 0:
      print_node(iter->first, "blue");
      break;
    case 1:
    case 2:
      print_node(iter->first, "orange");
      break;
    case 3:
      print_node(iter->first, "red");
      break;
    default:
      break;
    }
  }

  // print edges
  std::string s, label;
  //Vertex v;

    for(std::map<size_t,Vertex>::iterator iter=sparse_adj.begin();iter!=sparse_adj.end();++iter){
  //for (std::size_t i{}; i < this->V; i++) {
    //v = this->get_node(i);

    // unreachable
    if (iter->second.incoming.empty() && iter->second.outgoing.empty()) {
      // if (false) { std::cout << "\t" + print_idx(i) + ";\n"; }
      continue;
    }

    for (auto e : iter->second.outgoing) {
      std::cout << "\t" << print_idx(iter->first) << " -> " << print_idx(e.dest)
                << " [label=" << print_label(e)
                << ", weight=" << std::to_string(e.weight) << "];\n";
    }
  }

  std::cout << "}" << std::endl;
}


/**
 *
 * @param[in] g
 * @param[in] valid_matches
 *
 */
void graph::Graph::create_edge(
  std::vector<graph::GraphSlice> const &valid_matches,
  int qry,
  graph::BoundaryUnion bounds,
  bool eps_edge) {

  int eps_side = 0;

  switch (qry) {
  case 1:
    // T_1 is the qry
    //int eps_side = 0;
    if (eps_edge) {eps_side = 2;}
    for (auto x : valid_matches) {
      this->add_edge(x.get_qry_start(),
                     x.get_txt_start(),
                     bounds,
                     x.get_match_typ(),
                     x.get_match_length(),
                     x.get_str(),
                     eps_side);
    }
    break;
  case 2:
    // T_2 is the qry
    if (eps_edge) { eps_side = 1; }
    for (auto x : valid_matches) {
      this->add_edge(x.get_txt_start(),
                     x.get_qry_start(),
                     bounds,
                     x.get_match_typ(),
                     x.get_match_length(),
                     x.get_str(),
                     eps_side);
    }
    break;
  default:
    break;
  };
}
