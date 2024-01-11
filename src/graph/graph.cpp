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

/*
  try {
    adj.resize(V);
  } catch (const std::bad_alloc & e) {
     std::double_t space_in_gb = static_cast<double>(this->V) / std::pow(1024.0, 3);;
     space_in_gb *= sizeof(Vertex);

     std::cerr << "Not enough memory. This graph needs over "
               << ceil(space_in_gb)
               << " GB of memory.\n";

    exit(-1);
  } catch (const std::length_error &) {
    std::cerr << "graph would be too large" << std::endl;
    exit(-1);
  }*/

}

// x in [1 - n1 -1], also l
// y in [1 - n2 -1], also k
// zero indexed
std::size_t graph::Graph::compute_index(std::size_t x, std::size_t y) {
  // return ((y+j)*N_1) + x+i;
  return (x * (this->N_2 + 1)) + y;
}

graph::Vertex & graph::Graph::get_node(std::size_t node_idx) {
    std::map<size_t,graph::Vertex>::iterator it=sparse_adj.find(node_idx);
    if(it!=sparse_adj.end()){
        return it->second;
    }else{
        graph::Vertex v;
        sparse_adj.insert(std::make_pair(node_idx,v));
        return sparse_adj[node_idx];
    }
  //return this->adj[node_idx];
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
  //std::size_t last_node = this->last_node();

  //std::vector<size_t> dp_tbl(this->get_size(), 0);
  std::map<size_t,size_t> dp_map;
  std::size_t max_k = 0;

  // Note that this expects that idx will overflow when we attempt to go below
  // zero therefore assumes the last node is not max size_t value
  for(std::map<size_t,Vertex>::reverse_iterator riter=sparse_adj.rbegin();riter!=sparse_adj.rend();++riter){
  //for (std::size_t idx{this->last_node()}; idx <= this->last_node(); idx--) {
    //Vertex const& v = this->get_node(idx);

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

void graph::Graph::dbg_print(int indent_level = 0) {
  std::cerr << core::indent(indent_level)
            << "Graph info {" << std::endl
            << core::indent(indent_level + 1) << "nodes in " << core::T_1
            << ": " << this->N_1 << std::endl
            << core::indent(indent_level + 1) << "nodes in " << core::T_2
            << ": " << this->N_2 << std::endl
            << core::indent(indent_level + 1)
            << "size of uncompacted intersection graph: " << this->V << std::endl
            << core::indent(indent_level) << "}" << std::endl;
}

void graph::Graph::print_dot() {
  auto print_idx = [&](std::size_t idx) -> std::string {
    if (idx == 0) { return core::q_0; }
    if (idx == this->V - 1) { return core::q_a; }
    return std::to_string(idx);
  };


  auto print_node_label = [&](std::size_t idx) -> std::string {
    if (idx == 0) { return core::q_0; }
    if (idx == this->V - 1) { return core::q_a; }
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
      return " \"(" + core::unicode_eps + ", " + std::to_string(e.weight) +
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
 * Validate matches
 * find out where they start and end within the query and the text
 *
 * active suffixes exist in N
 *
 * a match is valid if:
 *   -
 *   -
 *
 *
 * txt_slices exist in l start and length
 * txt_offsets exist in N start and stop indexes of a str in N
 * qry_offsets exist in N start and stop indexes of a str in N
 * return
*/
void filter_matches(std::vector<core::EDSMatch> const &candidate_matches,
                    eds::EDS &txt_eds,
                    core::ed_string_e txt,
                    std::vector<eds::slice_eds> const &qry_offsets,
                    core::bool_matrix *txt_active_suffixes,
                    core::bool_matrix *qry_active_suffixes,
                    int qry_letter_idx,
                    int txt_letter_idx,
                    std::vector<graph::GraphSlice>* valid_matches,
                    core::AppConfig const &app_config) {
  for (auto candiate_match : candidate_matches) {
    eds::slice_eds local_txt_slice = txt_eds.get_str_slice_local(txt_letter_idx, candiate_match.get_txt_str_idx());

    /*
      evaluate the start of the match
      ===============================
     */

    // explicit or implicit match
    graph::match_type g_t_m_start, g_t_m_stop, g_q_m_start, g_q_m_stop;

    int t_start_in_N = txt_eds.to_global_idx(txt_letter_idx, candiate_match.get_char_idx()) + local_txt_slice.start;

    if (app_config.constraint) {
      // is the active suffix valid?
      bool valid_as{false};

      // the match starts within a string in the text
      // second condition because actv suff can only be extended ...
      if (candiate_match.get_char_idx() > 0 && qry_letter_idx > 0) {
        valid_as = ((*txt_active_suffixes)[qry_letter_idx - 1][t_start_in_N - 1] == 1);
      }

      // is an exp - exp match
      // or valid active suffix
      // txt start is not valid so skip
      if (candiate_match.get_char_idx() > 0 && !valid_as) { continue; }
    }

    g_t_m_start = candiate_match.get_char_idx() == 0 ? graph::match_type::exp : graph::match_type::imp;

    /*
      evaluate the END of the match
      ===============================
    */

    // where the match ends
    std::size_t candidate_match_end =
      local_txt_slice.start + candiate_match.get_char_idx() + candiate_match.get_match_length();

    // where the matched/text string actually ends
    std::size_t txt_slice_end = local_txt_slice.start + local_txt_slice.length;

    if (candidate_match_end >= txt_slice_end) {
      // the match went beyond the txt so we limit it to the end
      // a match that is beyond the text also lies here
      g_t_m_stop = graph::match_type::exp;
    } else {
      // the match ends within the text
      g_t_m_stop = graph::match_type::imp;

      // set the match end as an active suffix
      std::size_t t_end_in_N = txt_eds.to_global_idx(txt_letter_idx, candidate_match_end);
      (*txt_active_suffixes)[qry_letter_idx][t_end_in_N] = 1;
    }

    /*
      Handle query
      ============

      queries always start at 0 so we don't need to do any additional stuff
     */

    // determine match start and ends in N in the query EDS
    int q_start_in_N = qry_offsets[candiate_match.query_str_index].start;
    std::size_t qlen = qry_offsets[candiate_match.query_str_index].length;

    bool is_beyond_txt {candidate_match_end > txt_slice_end};

    g_q_m_start = graph::match_type::exp;
    g_q_m_stop = is_beyond_txt || (candiate_match.get_match_length() < qlen) ? graph::match_type::imp : graph::match_type::exp;

    if (app_config.constraint && g_q_m_stop == graph::match_type::imp && g_t_m_stop == graph::match_type::imp) {
      continue;
    }

    // create an active suffix in the query
    if (q_start_in_N + candiate_match.get_match_length() < q_start_in_N + qlen) {
      (*qry_active_suffixes)[txt_letter_idx][q_start_in_N + candiate_match.get_match_length() - 1 ] = 1;
    }

    graph::MatchTypePairUnion u = txt == core::ed_string_e::t1
      ? graph::MatchTypePairUnion(g_t_m_start, g_t_m_stop, g_q_m_start, g_q_m_stop)
      : graph::MatchTypePairUnion( g_q_m_start, g_q_m_stop, g_t_m_start, g_t_m_stop);

    valid_matches->push_back(
      graph::GraphSlice(t_start_in_N,
                        q_start_in_N,
                        u,
                        candiate_match.get_match_length(),
                        candiate_match.str));
  }
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
  bool eps_edge = false) {

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

// a suffix tree and the string it was built from as well
//struct meta_st {};

/**
*
*
* @param[in] eds_t1 T1
* @param[in] eds_t2 T2
* @param[in] app_config
* return an intersection graph
*/
graph::Graph graph::compute_intersection_graph(eds::EDS &eds_t1,
                                               eds::EDS &eds_t2,
                                               core::AppConfig const &app_config) {
  size_t size_t1 = eds_t1.get_size();
  size_t size_t2 = eds_t2.get_size();

  std::size_t len_t1 = eds_t1.get_length();
  std::size_t len_t2 = eds_t2.get_length();

  /*
    Generate suffix trees
    ---------------------
  */

  std::vector<match_st::meta_st> w_suffix_trees;
  w_suffix_trees.reserve(len_t1);
  std::vector<match_st::meta_st> q_suffix_trees;
  q_suffix_trees.reserve(len_t2);

  match_st::gen_suffix_tree_(eds_t1, &w_suffix_trees);
  match_st::gen_suffix_tree_(eds_t2, &q_suffix_trees);

  // active suffixes in N_1
  core::bool_matrix i_active_suffixes = core::gen_matrix(len_t2, size_t1 + eds_t1.get_eps_count() + 1);
  // active suffixes in N_2
  core::bool_matrix j_active_suffixes = core::gen_matrix(len_t1, size_t2 + eds_t2.get_eps_count() + 1);

  std::vector<core::EDSMatch> candidate_matches;
  candidate_matches.reserve(100);

  std::vector<graph::GraphSlice> valid_matches;
  valid_matches.reserve(1000);

  auto clean_up = [&candidate_matches, &valid_matches]() {
    candidate_matches.clear();
    valid_matches.clear();
  };

  /*
    Compute Graph
    -------------
  */

  graph::Graph g = graph::Graph(eds_t1.get_size() + eds_t1.get_eps_count(),
                                eds_t2.get_size() + eds_t2.get_eps_count());

  if (app_config.verbosity() > 0) { g.dbg_print(); }

  eds::LetterBoundary j_boundary;
  eds::LetterBoundary i_boundary;

  for (std::size_t i = 0; i < len_t1; i++) {
    for (std::size_t j = 0; j < len_t2; j++) {

      j_boundary = eds_t2.get_letter_boundaries(j);
      i_boundary = eds_t1.get_letter_boundaries(i);

      graph::BoundaryUnion bounds = graph::BoundaryUnion(i_boundary, j_boundary);

      /*
        Handle epsilon (Ɛ)
        -------------------
       */

      // T_2[j] has epsilon
      // T_1[i] is the query
      if (eds_t2.is_letter_eps(j)) {

        std::size_t eps_idx = eds_t2.get_global_eps_idx(j);

        graph::MatchTypePairUnion u =
            graph::MatchTypePairUnion(graph::match_type::exp,
                                      graph::match_type::exp,
                                      graph::match_type::exp,
                                      graph::match_type::exp);

        valid_matches.push_back(graph::GraphSlice(eps_idx, i_boundary.left(), u));

        // the last letter in T_1
        if (i == len_t1 - 1) {
          std::pair<int, int> last_i_boundary =
            std::make_pair(i_boundary.right() + 1, i_boundary.right() + 1);

          g.create_edge(valid_matches,
                        1,
                        graph::BoundaryUnion(last_i_boundary.first,
                                             last_i_boundary.second,
                                             j_boundary.left(),
                                             j_boundary.right()),
                        true);
        }

        // handle active suffixes
        if (j > 0) {
          for (std::size_t col=0; col <= i_boundary.right(); col++) {
            if (i_active_suffixes[j-1][col]) {
              // update active suffixes
              i_active_suffixes[j][col] = 1;

              graph::MatchTypePairUnion u =
                graph::MatchTypePairUnion(graph::match_type::imp,
                                          graph::match_type::imp,
                                          graph::match_type::exp,
                                          graph::match_type::exp);

              // update active suffixes
              valid_matches.push_back(graph::GraphSlice(eps_idx, col, u));
            }
          }
        }

        g.create_edge(valid_matches, 1, bounds, true);
      }

      clean_up();

      // T_1[i] has epsilon
      // T_2[j] is the query
      if (eds_t1.is_letter_eps(i)) {

        int eps_idx = eds_t1.get_global_eps_idx(i);

        graph::MatchTypePairUnion u =
            graph::MatchTypePairUnion(graph::match_type::exp,
                                      graph::match_type::exp,
                                      graph::match_type::exp,
                                      graph::match_type::exp);

        valid_matches.push_back(graph::GraphSlice(eps_idx, j_boundary.left(), u));

        // the last letter in T_2
        if (j == len_t2 - 1) {
          std::pair<int, int> last_j_boundary =
            std::make_pair(j_boundary.right() + 1, j_boundary.right() + 1);

          g.create_edge(valid_matches,
                        2,
                        graph::BoundaryUnion(i_boundary.left(),
                                             i_boundary.right(),
                                             last_j_boundary.first,
                                             last_j_boundary.second),
                        true);
        }

        // update active suffixes
        if (i > 0) {
          for (std::size_t col = 0; col <= j_boundary.right(); col++) {
            if (j_active_suffixes[i - 1][col]) {
              // update active suffixes
              j_active_suffixes[i][col] = 1;

              graph::MatchTypePairUnion u =
                graph::MatchTypePairUnion(graph::match_type::exp,
                                          graph::match_type::exp,
                                          graph::match_type::imp,
                                          graph::match_type::imp);

              // update active suffixes
              valid_matches.push_back(graph::GraphSlice(eps_idx, col, u));
            }
          }
        }

        g.create_edge(valid_matches, 2, bounds, true);
      }

      clean_up();

      /*
        Handle matching
        ---------------
       */

      // Search for j_strs in T_1[i]
      // Query => T_2[j]
      // Text => T_1[i]
      // ---------------------------

      core::perform_matching(eds_t1,
                             i,
                             j,
                             w_suffix_trees[i],
                             eds_t2.get_strs(j),
                             &candidate_matches,
                             true);

      core::mark_query_nodes(eds_t2,
                             j,
                             i,
                             q_suffix_trees[j],
                             candidate_matches);

      filter_matches(candidate_matches,
                     eds_t1,
                     core::ed_string_e::t1,
                     eds_t2.get_slice(j),
                     &i_active_suffixes,
                     &j_active_suffixes,
                     j,
                     i,
                     &valid_matches,
                     app_config);

      g.create_edge(valid_matches, 2, bounds);

       clean_up();

       // Search for i_strs in T_2[j]
       // Query => T_1[i]
       // Text => T_2[j]
       // ---------------------------

       core::perform_matching(eds_t2,
                              j,
                              i,
                              q_suffix_trees[j],
                              eds_t1.get_strs(i),
                              &candidate_matches,
                              true);

       core::mark_query_nodes(eds_t1,
                              i,
                              j,
                              w_suffix_trees[i],
                              candidate_matches);

       filter_matches(candidate_matches,
                      eds_t2,
                      core::ed_string_e::t2,
                      eds_t1.get_slice(i),
                      &j_active_suffixes,
                      &i_active_suffixes,
                      i,
                      j,
                      &valid_matches,
                      app_config);

       g.create_edge(valid_matches, 1, bounds);

       clean_up();
    }
  }

  return g;
}

/**
 * Compute MS[i] where i is the longest suffix starting at {i ∈ [0,n-1]} in
 * either T_1 or T_2 and is a substring of the other (T_2 or T_1)
 *
 * @param[in] g the graph
 * @param[in] letter_start the start of i in N
 * @param[in] last the last positon in the other ed string
 * @param[in] match_stats_str the string in which i lies (either T_1 or T_2)
 */
std::size_t graph::match_stats(graph::Graph &g,
                        std::size_t letter_start,
                        std::size_t last,
                        core::ed_string_e match_stats_str) {
  g.compute_match_stats();

  std::size_t max = 0;
  std::size_t node_idx{std::numeric_limits<std::size_t>::max()};

  for (std::size_t i{}; i<last; i++) {
    node_idx = match_stats_str == core::ed_string_e::t1
             ? g.compute_index(letter_start, i)
             : g.compute_index(i, letter_start);

    if (max < g.get_match_stats(node_idx)) {
      max = g.get_match_stats(node_idx);
    }
  }

  return max;
}

/**
 * Matching statistics from self to self
 *
 */
std::double_t match_stats_self(eds::EDS& e) {
  std::size_t n = e.get_length();

  // a vector of the longest string in the set starting at i-th letter
  std::vector<std::size_t> t1_letter_maxes;
  t1_letter_maxes.reserve(n);

  std::size_t longest_str_len{}; // the length of longest string in the set
  for (std::size_t i{}; i < n; i++) {
    longest_str_len = 0;
    // loop through all slices in the i-th letter
    for (eds::slice_eds sl : e.get_slice(i)) {
      if (sl.length > longest_str_len) { longest_str_len = sl.length; }
    }

    t1_letter_maxes.push_back(longest_str_len);
  }

  // sum up the elements of the vector from the back such that in the end
  // the element at i is set to the sum of the elements at  i and at i+1
  for (std::size_t i{n-2}; i < n-1 ; i--) {
    t1_letter_maxes[i] += t1_letter_maxes[i+1];
  }

  std::size_t self_prefix_sum =
    accumulate(t1_letter_maxes.begin(), t1_letter_maxes.end(), 0);

  //std::cout << "self prefix sum = " << self_prefix_sum << "\n";

  std::double_t ms_t1_t1 =
    static_cast<std::double_t>(self_prefix_sum) / static_cast<std::double_t>(n);

  return ms_t1_t1;
}

std::double_t graph::match_stats_avg_normalized(graph::Graph &g, eds::EDS& eds_t1, eds::EDS& eds_t2) {
  g.compute_match_stats();

  // the last letter in T_1 and T_2
  std::size_t len_t1 = eds_t1.get_length();
  std::size_t len_t2 = eds_t2.get_length();

  // the last value in internal size of T1 and T2
  std::size_t last_w = eds_t1.get_size() + eds_t1.get_eps_count();
  std::size_t last_q = eds_t2.get_size() + eds_t2.get_eps_count();

  // MS_T1 and MS_T2
  std::valarray<std::size_t> ms_w(len_t1), ms_q(len_t2);

  std::size_t max{}, i_start{}, j_start{};

  std::size_t node_idx{std::numeric_limits<std::size_t>::max()};

  for (std::size_t i{}; i < len_t1; i++) {
    i_start = eds_t1.get_letter_boundaries(i).left();
    for (std::size_t l{}; l < last_q; l++) {
      node_idx = g.compute_index(i_start, l);

      if (max < g.get_match_stats(node_idx)) {
        max = g.get_match_stats(node_idx);
      }
    }

    ms_w[i] = max;
    max = 0;
  }

  for (std::size_t j{}; j < len_t2; j++) {
    j_start = eds_t2.get_letter_boundaries(j).left();
    for (std::size_t k{}; k < last_w; k++) {
      node_idx = g.compute_index(k, j_start);

      if (max < g.get_match_stats(node_idx)) {
        max = g.get_match_stats(node_idx);
      }
    }

    ms_q[j] = max;
    max = 0;
  }

  // print ms_w
  /*
  std::cout << "ms_w = ";
  for (std::size_t i{}; i < len_t1; i++) {
    std::cout << ms_w[i] << " ";
    }
  std::cout << "\n";
  // print ms_q
  std::cout << "ms_q = ";
  for (std::size_t i{}; i < len_t2; i++) {
    std::cout << ms_q[i] << " ";
    }
  */

  std::double_t N_1 = static_cast<std::double_t>(eds_t1.get_size());
  std::double_t N_2 = static_cast<std::double_t>(eds_t2.get_size());

  std::double_t log_N_1 = std::log2(N_1);
  std::double_t log_N_2 = std::log2(N_2);

  std::double_t ms_t1_t1 = match_stats_self(eds_t1);
  std::double_t ms_t2_t2 = match_stats_self(eds_t2);

  // MS_T1_T2
  std::double_t ms_t1_t2 =
    static_cast<std::double_t>( ms_w.sum()) / static_cast<std::double_t>(len_t1);
  std::double_t ms_t2_t1 =
    static_cast<std::double_t>( ms_q.sum()) / static_cast<std::double_t>(len_t2);

  /*
  std::cout << "ms_t1_sum " << ms_w.sum() << " ms_t2_sum " << ms_q.sum()
            << " ms_t1_t2 " << ms_t1_t2 << " ms_t2_t1 " << ms_t2_t1
            << " ms_t1_t1 " << ms_t1_t1 << " ms_t2_t2 " << ms_t2_t2
            << " log_N_1 " << log_N_1 << " log_N_2: " << log_N_2
            << "\n";
  */

  std::double_t  d_t1_t2 = log_N_2 / ms_t1_t2 - log_N_1/ ms_t1_t1;
  std::double_t  d_t2_t1 = log_N_1 / ms_t2_t1 - log_N_2/ ms_t2_t2;

  //return ms_t1_t2 + ms_t2_t1;

  return (d_t1_t2 + d_t2_t1) / 2.0;
}

std::double_t graph::match_stats_avg(graph::Graph &g, eds::EDS& eds_t1, eds::EDS& eds_t2) {
  g.compute_match_stats();

  // the last letter in T_1 and T_2
  std::size_t len_t1 = eds_t1.get_length();
  std::size_t len_t2 = eds_t2.get_length();

  // the last value in internal size of T1 and T2
  std::size_t last_w = eds_t1.get_size() + eds_t1.get_eps_count();
  std::size_t last_q = eds_t2.get_size() + eds_t2.get_eps_count();

  // MS_T1 and MS_T2
  std::valarray<std::size_t> ms_w(len_t1), ms_q(len_t2);

  std::size_t max{}, i_start{}, j_start{};

  std::size_t node_idx{std::numeric_limits<std::size_t>::max()};

  for (std::size_t i{}; i < len_t1; i++) {
    i_start = eds_t1.get_letter_boundaries(i).left();
    for (std::size_t l{}; l < last_q; l++) {
      node_idx = g.compute_index(i_start, l);

      if (max < g.get_match_stats(node_idx)) {
        max = g.get_match_stats(node_idx);
      }
    }

    ms_w[i] = max;
    max = 0;
  }

  for (std::size_t j{}; j < len_t2; j++) {
    j_start = eds_t2.get_letter_boundaries(j).left();
    for (std::size_t k{}; k < last_w; k++) {
      node_idx = g.compute_index(k, j_start);

      if (max < g.get_match_stats(node_idx)) {
        max = g.get_match_stats(node_idx);
      }
    }

    ms_q[j] = max;
    max = 0;
  }


  //std::size_t sum_vals = ms_q.sum() + ms_w.sum();
  //std::size_t sum_size = len_t1 + len_t2;


  //return static_cast<std::double_t>(sum_vals) / static_cast<std::double_t>(sum_size);

  std::double_t t1_avg = static_cast<std::double_t>(ms_w.sum()) / static_cast<std::double_t>(len_t1);
  std::double_t t2_avg = static_cast<std::double_t>(ms_q.sum()) / static_cast<std::double_t>(len_t2);

  return t1_avg + t2_avg;
}

std::double_t graph::distance(graph::Graph &g, eds::EDS& eds_t1, eds::EDS& eds_t2) {
  g.compute_match_stats();

  // the last letter in T_1 and T_2
  std::size_t len_t1 = eds_t1.get_length();
  std::size_t len_t2 = eds_t2.get_length();

  // the last value in internal size of T1 and T2
  std::size_t last_w = eds_t1.get_size() + eds_t1.get_eps_count();
  std::size_t last_q = eds_t2.get_size() + eds_t2.get_eps_count();

  // MS_T1 and MS_T2
  std::valarray<std::size_t> ms_w(len_t1), ms_q(len_t2);

  std::size_t max{}, i_start{}, j_start{};

  std::size_t node_idx{std::numeric_limits<std::size_t>::max()};

  for (std::size_t i{}; i < len_t1; i++) {
    i_start = eds_t1.get_letter_boundaries(i).left();
    for (std::size_t l{}; l < last_q; l++) {
      node_idx = g.compute_index(i_start, l);

      if(g.check_match_stats(node_idx)){
            if (max < g.get_match_stats(node_idx)) {
                max = g.get_match_stats(node_idx);
            }
        }
    }

    ms_w[i] = max;
    max = 0;
  }

  for (std::size_t j{}; j < len_t2; j++) {
    j_start = eds_t2.get_letter_boundaries(j).left();
    for (std::size_t k{}; k < last_w; k++) {
      node_idx = g.compute_index(k, j_start);

      if(g.check_match_stats(node_idx)){
            if (max < g.get_match_stats(node_idx)) {
                max = g.get_match_stats(node_idx);
            }
        }
    }

    ms_q[j] = max;
    max = 0;
  }

  // print ms_w
  /*
  std::cout << "ms_w = ";
  for (std::size_t i{}; i < len_t1; i++) {
    std::cout << ms_w[i] << " ";
    }
  std::cout << "\n";
  // print ms_q
  std::cout << "ms_q = ";
  for (std::size_t i{}; i < len_t2; i++) {
    std::cout << ms_q[i] << " ";
    }
  */

  std::double_t N_1 = static_cast<std::double_t>(eds_t1.get_size());
  std::double_t N_2 = static_cast<std::double_t>(eds_t2.get_size());

  std::double_t log_N_1 = std::log2(N_1);
  std::double_t log_N_2 = std::log2(N_2);

  std::double_t ms_t1_t1 = match_stats_self(eds_t1);
  std::double_t ms_t2_t2 = match_stats_self(eds_t2);

  // MS_T1_T2
  std::double_t ms_t1_t2 =
    static_cast<std::double_t>( ms_w.sum()) / static_cast<std::double_t>(len_t1);
  std::double_t ms_t2_t1 =
    static_cast<std::double_t>( ms_q.sum()) / static_cast<std::double_t>(len_t2);

  /*
  std::cout << "ms_t1_sum " << ms_w.sum() << " ms_t2_sum " << ms_q.sum()
            << " ms_t1_t2 " << ms_t1_t2 << " ms_t2_t1 " << ms_t2_t1
            << " ms_t1_t1 " << ms_t1_t1 << " ms_t2_t2 " << ms_t2_t2
            << " log_N_1 " << log_N_1 << " log_N_2: " << log_N_2
            << "\n";
  */

  std::double_t  d_t1_t2 = log_N_2 / ms_t1_t2 - log_N_1/ ms_t1_t1;
  std::double_t  d_t2_t1 = log_N_1 / ms_t2_t1 - log_N_2/ ms_t2_t2;

  //return ms_t1_t2 + ms_t2_t1;

  return (d_t1_t2 + d_t2_t1) / 2.0;
}

std::double_t graph::similarity(graph::Graph &g, eds::EDS& eds_t1, eds::EDS& eds_t2) {
  g.compute_match_stats();

  // the last letter in T_1 and T_2
  std::size_t len_t1 = eds_t1.get_length();
  std::size_t len_t2 = eds_t2.get_length();

  // the last value in internal size of T1 and T2
  std::size_t last_w = eds_t1.get_size() + eds_t1.get_eps_count();
  std::size_t last_q = eds_t2.get_size() + eds_t2.get_eps_count();

  // MS_T1 and MS_T2
  std::valarray<std::size_t> ms_w(len_t1), ms_q(len_t2);

  std::size_t max{}, i_start{}, j_start{};

  std::size_t node_idx{std::numeric_limits<std::size_t>::max()};

  for (std::size_t i{}; i < len_t1; i++) {
    i_start = eds_t1.get_letter_boundaries(i).left();
    for (std::size_t l{}; l < last_q; l++) {
      node_idx = g.compute_index(i_start, l);
      if(g.check_match_stats(node_idx)){
            if (max < g.get_match_stats(node_idx)) {
                max = g.get_match_stats(node_idx);
            }
        }
    }

    ms_w[i] = max;
    max = 0;
  }

  for (std::size_t j{}; j < len_t2; j++) {
    j_start = eds_t2.get_letter_boundaries(j).left();
    for (std::size_t k{}; k < last_w; k++) {
      node_idx = g.compute_index(k, j_start);

      if(g.check_match_stats(node_idx)){
            if (max < g.get_match_stats(node_idx)) {
                max = g.get_match_stats(node_idx);
            }
        }
    }

    ms_q[j] = max;
    max = 0;
  }

#ifdef DEBUG
    std::cout<< "MS[S_1,S_2]: ";
    for(int i=0;i<len_t1;++i){ std::cout << ms_w[i]<<" "; }
    std::cout<<std::endl;
    std::cout<< "MS[S_2,S_1]: ";
    for(int i=0;i<len_t2;++i){ std::cout << ms_q[i]<<" "; }
    std::cout<<std::endl;
#endif

  std::double_t t1_avg = static_cast<std::double_t>(ms_w.sum()) / static_cast<std::double_t>(len_t1);
  std::double_t t2_avg = static_cast<std::double_t>(ms_q.sum()) / static_cast<std::double_t>(len_t2);

  return t1_avg + t2_avg;
}

int graph::longest_witness(graph::Graph &g) {
  return g.witness(0, g.get_size() - 1 );
}

std::size_t graph::multiset(graph::Graph &g) {
  return g.multiset_size();
}

int graph::shortest_witness(graph::Graph& g) {
  return g.dijkstra(0, g.get_size() - 1);
}
