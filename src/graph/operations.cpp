#include "./graph.hpp"

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

  for (std::size_t i{}; i < len_t1; i++) {
    for (std::size_t j{}; j < len_t2; j++) {

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

#ifdef DEBUG
  std::cerr << "BMS [S_1,S_2]: ";
  for (std::size_t i{}; i < len_t1; i++) { std::cerr << ms_w[i] << " "; }
  std::cerr << std::endl;
  std::cerr << "BMS [S_2,S_1]: ";
  for (std::size_t i{}; i < len_t2; i++) { std::cerr << ms_q[i] << " "; }
  std::cerr << std::endl;
#endif

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
    std::cerr << "MS [S_1,S_2]: ";
    for(std::size_t i{}; i<len_t1; ++i){ std::cerr << ms_w[i]<<" "; }
    std::cerr <<std::endl;
    std::cerr << "MS [S_2,S_1]: ";
    for(std::size_t i{}; i<len_t2; ++i){ std::cerr << ms_q[i]<<" "; }
    std::cerr<<std::endl;
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
