#include <cstddef>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <valarray>
#include <vector>

#include "../src/core/core.hpp"
#include "../src/eds/eds.hpp"
#include "../src/graph/graph.hpp"

TEST(GraphTest, ComputeGraph) {

  eds::EDS w, q;
  std::string input_string{""};
  int shortest, longest;
  std::string ed_string_w, ed_string_q;
  
  n_core::Parameters params;
  params.verbosity = 0;

  auto setup_and_run = [&]() {
    w = eds::Parser::from_string(ed_string_w);
    q = eds::Parser::from_string(ed_string_q);
    graph::Graph g = graph::compute_intersection_graph(w, q, params);
    longest = graph::longest_witness(g);
    shortest = graph::longest_witness(g);
  };

  ed_string_w = "CT{G,C}CACC";
  ed_string_q = "C{TG,AT,A}CACC";
  setup_and_run();
  EXPECT_EQ(longest, 7);
  EXPECT_EQ(shortest, 7);
  //EXPECT_EQ(shortest, longest);

  ed_string_w = "{GCG}{TTA,}{GTC}{G,T}";
  ed_string_q = "{GCGTT}{AG,T}{T}{AC,CG}";
  setup_and_run();

  ed_string_w = "TCTC{TCG,CGC}AACAATCACTCAA";
  ed_string_q = "TCTC{TC,CG}GAACA{A,,C}TCAC{TC,CT,A}AA";
  setup_and_run();


  ed_string_w = "{CCG}{CC,G}{AGA}";
  ed_string_q = "{CCGC}{C,GAGG}{AGA}";
  setup_and_run();

  ed_string_w = "{CAGG}{A,}{GAGGCACAATGAGCCAGCA}";
  ed_string_q = "{CAGGAG}{A,}{GGCACAATGAGCCAGCA}";
  setup_and_run();

  ed_string_w = "{AGG}{A,}{GAGG}";
  ed_string_q = "{AGGAG}{A,}{GG}";
  setup_and_run();

  ed_string_w = "{TTC}{A,GC}{CATT}{AC,}{ACTGGCCCGCGCGGTGG}";
  ed_string_q = "{TTCAC}{AT,C}{C,T}{AC}{T,}{TG}{A,}{GCCCGCGCGGTGGT}";
  setup_and_run();
  EXPECT_EQ(longest, -1);
  EXPECT_EQ(shortest, -1);
  //EXPECT_NE(shortest, longest);

  ed_string_w = "{TTC}{A,GC}{CATT}{AC,}{ACTGGCCC}";
  ed_string_q = "{TTCAC}{AT,C}{C,T}{AC}{T,}{TG}{A,}{GCCC}";
  setup_and_run();
  EXPECT_EQ(longest, 16);
  EXPECT_EQ(shortest, 16);
  //EXPECT_EQ(shortest, longest);

  ed_string_w = "AATCAGTGCTTCTAGCTCTTGGAGGGCTTGTACATTAACGGAACT{G,C}CA";
  ed_string_q = "AATCAGTGCTTCTAGCTCTTGGAGGGCTTGTACATTAACGGAAC{TG,AT,A}CA";
  setup_and_run();

  ed_string_w = "GTGACATGTACATTACT{G,C}CA";
  ed_string_q = "GTGACATGTACATTAC{TG,AT,A}CA";
  setup_and_run();

  /*
    full matches
    ------------
  */

  // TODO: ?
  ed_string_w = "{AT,TC}";
  ed_string_q = "{,G}";
  setup_and_run();

  ed_string_w = "{AT,TC}";
  ed_string_q = "{TC,G}";
  setup_and_run();

  ed_string_w = "{ACT}";
  ed_string_q = "{A}{C}{T}";
  setup_and_run();

  ed_string_w = "{AT,TC}{AC,T}";
  ed_string_q = "{TC,G}{CT,T}";
  setup_and_run();

  // active suffixes
  ed_string_w = "{AT,TC}{ATC,T}";
  ed_string_q = "{TC,G}{CT,T}";
  setup_and_run();
  EXPECT_EQ(longest, 3);
  EXPECT_EQ(shortest, 3);
  //EXPECT_EQ(shortest, longest);

  /*
  epsilons
  --------
 */

  ed_string_w = "{T}{C}";
  ed_string_q = "T{,G}{C}";
  setup_and_run();
  EXPECT_EQ(longest, 2);
  EXPECT_EQ(shortest, 2);
  EXPECT_EQ(shortest, longest);

  

  ed_string_w = "{AT,TC}{ATC,T}";
  ed_string_q = "TC{,G}{CT,T}";

  setup_and_run();

  ed_string_w = "{GT,}{A,C}{T}{A,G}{AT}";
  ed_string_q = "{AT,}{GAT}";
  setup_and_run();

  ed_string_w = "{AT,TC}{ATC,T}";
  ed_string_q = "{,G}AT{CT,T}";
  setup_and_run();

  ed_string_w = "{TTGA}";
  ed_string_q = "{TT}{A,}{GA}";
  setup_and_run();

  ed_string_w = "{T}";
  ed_string_q = "{T}{A,}";
  setup_and_run();

  ed_string_w = "{TG}";
  ed_string_q = "{T}{A,}{G}";
  setup_and_run();

  ed_string_w = "{TTGAATATGGCATTC}";
  ed_string_q = "{TT}{A,}{GAATATGGCATTC}";
  setup_and_run();

  ed_string_w = "{GT,}{A,C}{T}{A,G}{ATGCGCTT}{TG,}{TGTTG}{GC,}{GCGGTGGCTT}{AC,G}";
  ed_string_q = "{AT,}{GAT}{C,G}{C}{G,T}{C}{AGG,TT}{T}{G,T}{TGTTGGCGCGGTGGCTT}{AC,G}";
  setup_and_run();

  ed_string_w =
      "{TGCAGGGGCTAATCGACCTCTGGC}{A,}{AACCACTTTTCCATGAC}{A,}{AGGA}"
      "{G,}{TT}{A,}{GAATATGGCATTCAGTAATCCCTTC}{GGCCG,}{GATGATCGCAGGGAGCGTT}";
  ed_string_q = "{TGCAGGGGCTAATCGACCTCTGGC}{A,}{AAC}{CAC,GGT}{TTTTCCATGAC}{A,}{"
                "AGGA}{G,}{TTGAATATGGCATTC}{A,C}{GTAATCCCT}{CG,}{TCGATGATC}{G,}"
                "{CA}{AGC,}{GGGAGCG}{TT,}";
  setup_and_run();


  ed_string_w = "{TCCCTTC}{GGCCG,}{G}";
  ed_string_q = "{TCCCT}{CG,}{TCG}";
  setup_and_run();

  ed_string_w = "{GTAATCCCTTC}{GGCCG,}{GATGATC}";
  ed_string_q = "{GTAATCCCT}{CG,}{TCGATGATC}";
  setup_and_run();

  ed_string_w = "{AT,TC}{ATC,}";
  ed_string_q = "{TC,G}{CT,T}";
  setup_and_run();

  ed_string_w = "{AGGA}{G,}{TTGAATATGGCATTC}{A,C}{GTAATCCCT}{CG,}{TCGATGATC}";
  ed_string_q = "{AGGA}{G,}{TT}{A,}{GAATATGGCATTCAGTAATCCCTTC}";
  setup_and_run();

  ed_string_w = "{TC}{G,}{G}";
  ed_string_q = "{T}{CG,}{TCG}";
  setup_and_run();
  // TODO: figure out this test
  EXPECT_EQ(longest, -1);
  EXPECT_EQ(shortest, -1);
  

  ed_string_w = "C{TG,AT,A}C";
  ed_string_q = "CT{G,C}C";
  setup_and_run();

  ed_string_w = "C{TG,AT,A}C";
  ed_string_q = "CT{G,C}C";
  setup_and_run();

  ed_string_w = "{GT,}{A,C}{T}{A,G}{ATGCGCTT}";
  ed_string_q = "{G,}{AT}{CC,GA}{T}{C,G}{A,C}{G}{C,G}{TT}";
  setup_and_run();

  ed_string_w = "{GCGGTGGCTT}{AC,G}";
  ed_string_q = "{G,T}{TGTTGGCGCGGTGGCTT}{AC,G}";
  setup_and_run();

  ed_string_w = "{GCG}{TTA,}{GTC}{G,T}";
  ed_string_q = "{GCGTT}{AG,T}{T}{AC,CG}";
  setup_and_run();

  ed_string_w = "{AGGA}{G,}{TTGAATATGGCATTC}{A,C}";
  ed_string_q = "{AGGA}{G,}{TT}{A,}{GAATATGGCATTCA}";
  setup_and_run();

  ed_string_w = "{AGGA}{G,}{TTGAATATGGCATTC}{A,C}{GTAATCCCT}{CG,}{TCGATGATC}";
  ed_string_q = "{AGGA}{G,}{TT}{A,}{GAATATGGCATTCAGTAATCCCTTC}";
  setup_and_run();

  ed_string_w = "{A,G}{CTC,CAAGT}{GA,CTA}";
  ed_string_q = "{ACTCGA}";
  setup_and_run();

  ed_string_w = "CT{G,C}CACC";
  ed_string_q = "C{TG,AT,A}CACC";
  setup_and_run();

}
