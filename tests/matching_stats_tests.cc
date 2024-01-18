#include <cstddef>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <valarray>
#include <vector>

#include "../src/core/core.hpp"
#include "../src/eds/eds.hpp"
#include "../src/graph/graph.hpp"


TEST(GraphTest, MatchStatsUnconstrained) {
  eds::EDS t1, t2;
  std::string input_string {};
  std::double_t similarity_measure;
  std::string ed_string_t1 {}, ed_string_t2 {};

  core::AppConfig config;
  config.set_verbosity(0);

  auto setup_and_run = [&]() {
    t1 = eds::Parser::from_string(ed_string_t1);
    t2 = eds::Parser::from_string(ed_string_t2);

    graph::Graph g = graph::compute_intersection_graph(t1, t2, config);
    similarity_measure = graph::similarity(g, t1, t2);
  };

  ed_string_t1 = "{CCA,AAA}";
  ed_string_t2 = "{C,A}{CCC}";
  setup_and_run();
  EXPECT_EQ(similarity_measure, 4);

  ed_string_t1 = "{AACCA}{CCA,C,CAAAC}";
  ed_string_t2 = "{CA,CC,CCCA,CAAC}{ACA,CCAAC,AAAC,AAACA,AA}";
  setup_and_run();
  EXPECT_EQ(similarity_measure, 9);

  ed_string_t1 = "{AA}{AA}";
  ed_string_t2 = "{ACC,A,CA}{CAC,AAC}";
  setup_and_run();
  EXPECT_EQ(similarity_measure, 5);

  ed_string_t1 = "{A,AC,TGCT}{CA,}";
  ed_string_t2 = "{,T}{GCA,AC}";
  setup_and_run();
  EXPECT_EQ(similarity_measure, 5);

  ed_string_t1 = "{CCC}{ACC,CA,CAA}";
  ed_string_t2 = "{CAC,C}{AA,CA}";
  setup_and_run();
  EXPECT_EQ(similarity_measure, 5.5);

  ed_string_t1 = "{ACA,AC,AA}{C,AC,CAA}";
  ed_string_t2 = "{AC,AA}{CC,CA,CCC}{CC}{AC}";
  setup_and_run();
  EXPECT_EQ(similarity_measure, 6.0);

  ed_string_t1 = "{,C}{,CA}";
  ed_string_t2 = "{AC}{AC}";
  setup_and_run();
  EXPECT_EQ(similarity_measure, 3.0);

  ed_string_t1 = "{AC}{AC}";
  ed_string_t2 = "{,C}{,CA}";
  setup_and_run();
  EXPECT_EQ(similarity_measure, 3.0);

  ed_string_t1 = "{CC}{AA,CC}{A}";
  ed_string_t2 = "{CC,A}{CA,AC}{CA,AC}";
  setup_and_run();
  EXPECT_DOUBLE_EQ(similarity_measure, 5.6666666666666661);
}


TEST(GraphTest, MatchStatsConstrained) {
  eds::EDS t1, t2;
  std::string input_string {};
  double_t similarity_measure;
  std::string ed_string_t1 {}, ed_string_t2 {};

  core::AppConfig config;
  config.set_verbosity(0);
  config.constraint = 1;

  auto setup_and_run = [&]() {
    t1 = eds::Parser::from_string(ed_string_t1);
    t2 = eds::Parser::from_string(ed_string_t2);

    graph::Graph g = graph::compute_intersection_graph(t1, t2, config);
    similarity_measure = graph::similarity(g, t1, t2);
  };


  ed_string_t1 = "{AC}{CC}";
  ed_string_t2 = "{A,AC}";
  setup_and_run();
  EXPECT_EQ(similarity_measure, 3);


  ed_string_t1 = "{CCC}{CAA,ACC}{CAC,CC}";
  ed_string_t2 = "{AAC,AA,CA}{C}{CA,ACA}";
  setup_and_run();
  // maybe use expect near eq ? https://stackoverflow.com/a/15128658
  EXPECT_DOUBLE_EQ(similarity_measure,  4.6666666666666661);
}
