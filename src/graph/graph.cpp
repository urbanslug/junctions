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


  try {
    adj.resize(V);
  } catch (const std::bad_alloc &) {
    std::cerr << "not enough memory for this graph" << std::endl;
    exit(-1);
  } catch (const std::length_error &) {
    std::cerr << "graph would be too large" << std::endl;
    exit(-1);
  }
}

// x in [1 - n1 -1], also l
// y in [1 - n2 -1], also k
// zero indexed
std::size_t graph::Graph::compute_index(std::size_t x, std::size_t y) {
  // return ((y+j)*N_1) + x+i;
  return (x * (this->N_2 + 1)) + y;
}

graph::Vertex const& graph::Graph::get_node(std::size_t node_idx) {
  return this->adj[node_idx];
}

std::size_t graph::Graph::get_match_stats(std::size_t node_idx) {
  return this->match_stats[node_idx];
}




std::size_t graph::Graph::get_size() { return this->V; }

std::size_t graph::Graph::last_node() const { return this->V-1; }


// TODO: replace i & j boundary with a struct
/**
 * function to add an edge to the graph
 */
void graph::Graph::add_edge(std::size_t N_1,
                            std::size_t N_2,
                            std::pair<std::size_t, std::size_t> i_boundary,
                            std::pair<std::size_t, std::size_t> j_boundary,
                            MatchTypePairUnion m_typ,
                            std::size_t weight,
                            std::string str,
                            int eps_side = 0) {
  int stop, start;
  int l, k, l_prime, k_prime;


  l = m_typ.left1() == graph::match_type::exp ? i_boundary.first : N_1;
  k = m_typ.left2() == graph::match_type::exp ? j_boundary.first : N_2;

  start = compute_index(l, k);

  l_prime = m_typ.right1() == graph::match_type::exp ? i_boundary.second + 1 : N_1 + weight;
  k_prime = m_typ.right2() == graph::match_type::exp ? j_boundary.second + 1 : N_2 + weight;

  if (eps_side == 1) { k_prime = k; }

  if (eps_side == 2) { l_prime = l; }

  stop = compute_index(l_prime, k_prime);

  bool is_eps = (eps_side == 1 || eps_side == 2) ? true : false;

  if (is_eps ) { weight = 0; str = "";}

  graph::Edge e_rev = graph::Edge(start, weight, str, is_eps);
  graph::Edge e = graph::Edge(stop, weight, str, is_eps);

  adj[start].outgoing.insert(e);
  adj[stop].incoming.insert(e_rev);

  // start type, end type
  adj[start].vertex_type = m_typ.get_val_left();
  adj[stop].vertex_type = m_typ.get_val_right();
}

std::size_t graph::Graph::multiset_size() {
  std::vector<std::size_t> dp_table(this->V, 0);
  std::size_t start_node, stop_node;
  start_node = 0;
  stop_node = this->V - 1;
  dp_table[start_node] = 1;

  std::set<Edge> out;

  std::size_t current_node = start_node;

  std::stack<std::size_t> to_visit;
  std::set<std::size_t> visited;

  to_visit.push(current_node);

  while (!to_visit.empty()) {
    current_node = to_visit.top();
    to_visit.pop();

    if (visited.count(current_node) > 0) {
      continue;
    }

    out = this->adj[current_node].outgoing;
    for (auto e : out) {
      dp_table[e.dest] += dp_table[current_node];
      to_visit.push(e.dest);
    }
  }

  return dp_table[stop_node];
}


void graph::Graph::compute_match_stats() {
  //std::size_t last_node = this->last_node();

  std::vector<size_t> dp_tbl(this->get_size(), 0);
  std::size_t max_k = 0;

  // Note that this expects that idx will overflow when we attempt to go below
  // zero therefore assumes the last node is not max size_t value
  for (std::size_t idx{this->last_node()}; idx <= this->last_node(); idx--) {
    Vertex const& v = this->get_node(idx);

    for (auto e: v.outgoing) {
      if (max_k < (e.weight + dp_tbl[e.dest]) ) {
        max_k = e.weight + dp_tbl[e.dest];
      }
    }

    dp_tbl[idx] += max_k;
    max_k = 0;
  }

  this->match_stats = dp_tbl;
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
  std::vector<int> dists(this->V, INT_MIN);

  std::stack<std::size_t> to_visit;
  std::set<std::size_t> visited;
  std::set<Edge> out;
  int current_dist{};
  bool end_reached{false};

  std::size_t current_node = start_node_idx;
  dists[current_node] = 0;
  to_visit.push(current_node);


  while (!to_visit.empty()) {
    current_node = to_visit.top();
    to_visit.pop();
    out = this->adj[current_node].outgoing;

    if (visited.count(current_node) > 0) { continue; }

    current_dist = dists[current_node];

    for (auto e : out) {

      if (dists[tbl_idx(e.dest)] < current_dist + static_cast<int>(e.weight)) {
        if (e.dest == stop_node_idx) { end_reached = true; }
        dists[tbl_idx(e.dest)] = current_dist + static_cast<int>(e.weight);
      }

      to_visit.push(e.dest);
    }
    visited.insert(current_node);
  }

  if (end_reached) {
    return dists[tbl_idx(stop_node_idx)];
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
  std::vector<int> dists(this->V, INT_MAX);
  std::vector<bool> explored(this->V, false);

  bool end_reached{false};

  std::priority_queue<std::pair<std::size_t, std::size_t>, std::vector<std::pair<std::size_t, std::size_t>>,
                      compare_by_weight>
      min_queue;

  // set distance from start_node_idx to itself as 0
  dists[start_node_idx] = 0;

  min_queue.push(std::make_pair(start_node_idx, dists[start_node_idx]));

  // a pair of node idx and weight
  std::pair<std::size_t, std::size_t> u;
  std::size_t i; // i is the node index w is the weight

  while (!min_queue.empty()) {
    u = min_queue.top();
    i = u.first;

    if (i == stop_node_idx) {
      return dists[i];
    }

    explored[i] = true;
    min_queue.pop();

    for (auto ej : this->adj[i].outgoing) {

      // relax
      if (dists[i] + static_cast<int>(ej.weight) < dists[ej.dest]) {
        if (ej.dest == stop_node_idx) { end_reached = true; }
        dists[ej.dest] = dists[i] + ej.weight;
      }

      // add to min queue
      if (!explored[ej.dest]) {
        min_queue.push(std::make_pair(ej.dest, dists[ej.dest]));
      }
    }
  }

  if (end_reached) {
    return dists[stop_node_idx];
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
            << "size of intersection graph: " << this->V << std::endl
            << core::indent(indent_level) << "}" << std::endl;
}

void graph::Graph::print_dot() {
  auto print_idx = [&](std::size_t idx) -> std::string {
    if (idx == 0) { return core::q_0; }
    if (idx == this->V - 1) { return core::q_a; }
    return std::to_string(idx);
  };

  auto print_node = [&](std::size_t idx, std::string color) {
    if (idx == 0 || idx == this->V - 1) {
      color = "green";
    }
    std::cout << "\t" << print_idx(idx) << " [color=\"" << color << "\"];"
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
  for (std::size_t i{}; i < this->V; i++) {
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

  for (std::size_t i{}; i < this->V; i++) {
    v = this->adj[i];

    // unreachable
    if (v.incoming.empty() && v.outgoing.empty()) {
      // if (false) { std::cout << "\t" + print_idx(i) + ";\n"; }
      continue;
    }

    for (auto e : v.outgoing) {
      std::cout << "\t" << print_idx(i) << " -> " << print_idx(e.dest)
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
 * txt_slices exist in l start and length
 * txt_offsets exist in N start and stop indexes of a str in N
 * qry_offsets exist in N start and stop indexes of a str in N
 * return
*/
void filter_matches(std::vector<core::EDSMatch> const &candidate_matches,
                    eds::EDS &txt_eds,
                    core::ed_string txt,
                    std::vector<eds::slice_eds> const &qry_offsets,
                    core::bool_matrix *txt_active_suffixes,
                    core::bool_matrix *qry_active_suffixes,
                    int qry_letter_idx,
                    int txt_letter_idx,
                    std::vector<graph::GraphSlice>* valid_matches) {
  for (auto candiate_match : candidate_matches) {

    eds::slice_eds local_txt_slice =
      txt_eds.get_str_slice_local(txt_letter_idx,
                                  candiate_match.get_txt_str_idx());

    /*
      evaluate the start of the match
      ===============================
     */

    // explicit or implicit match
    graph::match_type g_t_m_start, g_t_m_stop, g_q_m_start, g_q_m_stop;

    // is the active suffix valid?
    bool valid_as{false};

    // TODO: rename t_start in N to something like match_start_in_N
    int t_start_in_N =
        txt_eds.to_global_idx(txt_letter_idx, candiate_match.get_char_idx()) +
        local_txt_slice.start;

    // the match starts within a string in the text
    // second condition because actv suff can only be extended ...
    if (candiate_match.get_char_idx() > 0 && qry_letter_idx > 0) {
      // is valid active suffix
      valid_as = ((*txt_active_suffixes)[qry_letter_idx - 1][t_start_in_N] == 1);
      bool shorter_match{false};
      int pos;
      for (std::size_t idx{1}; idx < candiate_match.get_match_length(); idx++) {
        if ((*txt_active_suffixes)[qry_letter_idx - 1][t_start_in_N + idx] == 1) {
          pos = idx;
          shorter_match = true;
          break;
        }
      }

      if (!valid_as && shorter_match) {
        t_start_in_N += pos;
        candiate_match.get_txt_char_idx_mut() += pos;
        candiate_match.get_match_length_mut() -= pos;
        candiate_match.get_str_mut() = candiate_match.str.substr(pos);
        valid_as = true;
      }
    }

    // is an exp - exp match
    // or valid active suffix
    // txt start is not valid so skip
    if (candiate_match.get_char_idx() > 0 && !valid_as) { continue; }

    if (candiate_match.get_char_idx() == 0) {
      //t_m_start = n_junctions::match_type::exp;
      g_t_m_start = graph::match_type::exp;
    } else {
      //t_m_start = n_junctions::match_type::imp;
      g_t_m_start = graph::match_type::imp;
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
    std::size_t candidate_match_end =
      candiate_match.get_char_idx() + candiate_match.get_match_length();

    // where the matched string actually ends
    std::size_t txt_slice_end = local_txt_slice.start + local_txt_slice.length;

    // find where the match ends within the txt string
    if (local_txt_slice.start + candidate_match_end >= txt_slice_end) {
      // the match went beyond the txt so we limit it to the end
      // a match that is beyond the text also lies here

      match_end = txt_slice_end; // actual idx is -1

      //m_len = match_end - (txt_slice.start + match_start_in_txt);
      //t_m_stop = n_junctions::match_type::exp;
      g_t_m_stop = graph::match_type::exp;
    } else {
      // within the text
      match_end = candidate_match_end; // actual idx is -1
      //m_len = candiate_match.match_length;

      //t_m_stop = n_junctions::match_type::imp;
      g_t_m_stop = graph::match_type::imp;

      // handle active suffix
      // this creates and active suffix
      // set match end as an active suffix
      std::size_t in_N = txt_eds.to_global_idx(txt_letter_idx, match_end);
      //int in_N = in_txt_N_2(candiate_match.get_txt_str_idx(), match_end);

      // std::cerr << utils::indent(2) << "save as: " << qry_letter_idx << in_N;
      (*txt_active_suffixes)[qry_letter_idx][in_N] = 1;
    }

    /*
      Handle query
      ============

      queries always start at 0 so we don't need to do any additional stuff
     */

    // determine match start and ends in N in the query EDS
    int q_start_in_N = qry_offsets[candiate_match.query_str_index].start;


    g_q_m_start = graph::match_type::exp;
    g_q_m_stop = candiate_match.is_beyond_txt() ? graph::match_type::imp
                                                : graph::match_type::exp;

    // create an active suffix in the query
    if (candiate_match.is_beyond_txt()) {
      (*qry_active_suffixes)[txt_letter_idx][q_start_in_N + candiate_match.get_match_length()] = 1;
    }


    graph::MatchTypePairUnion u = txt == core::ed_string::w
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
  std::pair<std::size_t, std::size_t> qry_boundary,
  std::pair<std::size_t, std::size_t> txt_boundary,
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
                     qry_boundary,
                     txt_boundary,
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
                     txt_boundary,
                     qry_boundary,
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

/**
*
*
* @param[in] eds_w
* @param[in] eds_q
* @param[in] parameters
* return
*/
graph::Graph graph::compute_intersection_graph(eds::EDS &eds_w, eds::EDS &eds_q,
                                 core::Parameters const &parameters) {

  size_t size_w = eds_w.get_size();
  size_t size_q = eds_q.get_size();

  std::size_t len_w = eds_w.get_length();
  std::size_t len_q = eds_q.get_length();

  /*
    Generate suffix trees
    ---------------------
  */

  std::vector<std::pair<match_st::STvertex, std::string>> w_suffix_trees;
  w_suffix_trees.reserve(len_w);
  std::vector<std::pair<match_st::STvertex, std::string>> q_suffix_trees;
  q_suffix_trees.reserve(len_q);

  gen_suffix_tree(eds_w, &w_suffix_trees);
  gen_suffix_tree(eds_q, &q_suffix_trees);

  // active suffixes in N_1
  core::bool_matrix i_active_suffixes = core::gen_matrix(len_q, size_w + eds_w.get_eps_count());
  // active suffixes in N_2
  core::bool_matrix j_active_suffixes = core::gen_matrix(len_w, size_q + eds_q.get_eps_count());

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

  graph::Graph g = graph::Graph(eds_w.get_size() + eds_w.get_eps_count(),
                                eds_q.get_size() + eds_q.get_eps_count());

  if (parameters.verbosity() > 0) { g.dbg_print(); }

  std::pair<std::size_t, std::size_t> j_boundary;
  std::pair<std::size_t, std::size_t> i_boundary;

  for (std::size_t i = 0; i < len_w; i++) {
    for (std::size_t j = 0; j < len_q; j++) {

      j_boundary = eds_q.get_letter_boundaries(j);
      i_boundary = eds_w.get_letter_boundaries(i);

      // T_2[j] has epsilon
      // T_1[i] is the query
      if (eds_q.is_letter_eps(j)) {
        // TODO: make size_t
        std::size_t eps_idx = eds_q.get_global_eps_idx(j);

        graph::MatchTypePairUnion u =
            graph::MatchTypePairUnion(graph::match_type::exp,
                                      graph::match_type::exp,
                                      graph::match_type::exp,
                                      graph::match_type::exp);


        valid_matches.push_back(graph::GraphSlice(eps_idx, i_boundary.first, u));

        // the last letter in T_1
        if (i == len_w - 1) {
          std::pair<int, int>  accept_qry_boundary = std::make_pair(i_boundary.second + 1, i_boundary.second + 1);
          g.create_edge(valid_matches, 1, accept_qry_boundary, j_boundary, true);
        }

        // handle active suffixes
        if (j > 0) {
          for (std::size_t col=0; col <= i_boundary.second; col++) {
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

        g.create_edge(valid_matches, 1, i_boundary, j_boundary, true);
      }

      clean_up();

      // T_1[i] has epsilon
      // T_2[j] is the query
      if (eds_w.is_letter_eps(i)) {

        int eps_idx = eds_w.get_global_eps_idx(i);

        graph::MatchTypePairUnion u =
            graph::MatchTypePairUnion(graph::match_type::exp,
                                      graph::match_type::exp,
                                      graph::match_type::exp,
                                      graph::match_type::exp);


        valid_matches.push_back(graph::GraphSlice(eps_idx, j_boundary.first, u));

        // the last letter in T_2
        if (j == len_q - 1) {
          std::pair<int, int> accept_qry_boundary =
              std::make_pair(j_boundary.second + 1, j_boundary.second + 1);
          g.create_edge(valid_matches, 2, accept_qry_boundary, i_boundary, true);
        }

        // update active suffixes
        if (i > 0) {
          for (std::size_t col = 0; col <= j_boundary.second; col++) {
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

        g.create_edge(valid_matches, 2, j_boundary, i_boundary, true);
      }

      clean_up();

      // Search for j_strs in T_1[i]
      // ---------------------------

      // Query => T_2[j]
      // Text => T_1[i]

      core::perform_matching(eds_w,
                               i,
                               &w_suffix_trees[i],
                               eds_q.get_strs(j),
                               &candidate_matches);

      filter_matches(candidate_matches,
                     eds_w,
                     core::ed_string::w,
                     eds_q.get_slice(j),
                     &i_active_suffixes,
                     &j_active_suffixes,
                     j,
                     i,
                     &valid_matches);


      g.create_edge(valid_matches, 2, j_boundary, i_boundary);

       clean_up();

       // Search for i_strs in T_2[j]
       // Query => T_1[i]
       // Text => T_2[j]
       // ---------------------------
       core::perform_matching(eds_q,
                                j,
                                &q_suffix_trees[j],
                                eds_w.get_strs(i),
                                &candidate_matches);

       filter_matches(candidate_matches,
                      eds_q,
                      core::ed_string::q,
                      eds_w.get_slice(i),
                      &j_active_suffixes,
                      &i_active_suffixes,
                      i,
                      j,
                      &valid_matches);

       g.create_edge(valid_matches, 1, i_boundary, j_boundary);

       clean_up();
    }
  }

  return g;
}

std::size_t graph::match_stats(graph::Graph &g,
                        std::size_t letter_start,
                        std::size_t last,
                        core::ed_string match_stats_str) {
  g.compute_match_stats();

  std::size_t max = 0;
  std::size_t node_idx{std::numeric_limits<std::size_t>::max()};

  for (std::size_t i{}; i<last; i++) {
    node_idx = match_stats_str == core::ed_string::w
             ? g.compute_index(letter_start, i)
             : g.compute_index(i, letter_start);

    if (max < g.get_match_stats(node_idx)) {
      max = g.get_match_stats(node_idx);
    }
  }

  return max;
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
