#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <valarray>
#include <vector>

//#include "../src/core/core.hpp"
#include "../src/eds/eds.hpp"
#include "../src/graph/graph.hpp"

TEST(GraphTest, ComputeGraph) {
  eds::EDS w, q;
  std::string input_string{""};

  std::string ed_string_w = "CT{G,C}CACC";
  std::string ed_string_q = "C{TG,AT,A}CACC";

  parse_data(ed_string_w, &w);
  parse_data(ed_string_q, &q);

  n_core::Parameters params;
  params.verbosity = 0;

  graph::Graph g = graph::compute_intersection_graph(w, q, params);
}
