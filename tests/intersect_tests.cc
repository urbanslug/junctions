#include <gtest/gtest.h>

#include "../src/core/core.hpp"
#include "../src/intersect/intersect.hpp"


TEST(IntersectionTest, LacksIntersection) {
  eds::EDS w, q;
  std::string ed_string_w, ed_string_q;
  bool res; // result
  core::AppConfig params;
  params.set_verbosity(0); // TODO: remove


  auto setup_and_run = [&]() {
    w = eds::Parser::from_string(ed_string_w);
    q = eds::Parser::from_string(ed_string_q);

    res = intersect::improved::has_intersection(w, q);
    // TODO: call naive
    //res = intersect::improved(w, q, params);
  };

  ed_string_w = "{AT,TC}{ATC,T}";
  ed_string_q = "{TC,G}{CG,G}";
  setup_and_run();
  EXPECT_EQ(res, false);
}


TEST(IntersectionTest, ActiveSuffixes) {
  eds::EDS w, q;
  std::string ed_string_w, ed_string_q;
  bool res; // result
  core::AppConfig params;
  params.set_verbosity(0); // TODO: remove

  auto setup_and_run = [&]() {
    w = eds::Parser::from_string(ed_string_w);
    q = eds::Parser::from_string(ed_string_q);

    res = intersect::improved::has_intersection(w, q);
    // TODO: call naive
    //res = intersect::improved(w, q, params);
  };

  ed_string_w = "AAGAA{AG}TCA";
  ed_string_q = "AA{GA}AAGTCA";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w = "CACCACGTACTTGACGTTACGAAAGAACAG";
  ed_string_q = "CACCACGTACTTGACGTTACGAAAGAA{CAG,AC}";
  setup_and_run();
  EXPECT_EQ(res, true);
}


TEST(IntersectionTest, Epsilons) {
  eds::EDS w, q;
  std::string ed_string_w, ed_string_q;
  bool res; // result
  core::AppConfig params;
  params.set_verbosity(0); // TODO: remove

  auto setup_and_run = [&]() {
    w = eds::Parser::from_string(ed_string_w);
    q = eds::Parser::from_string(ed_string_q);

    res = intersect::improved::has_intersection(w, q);
    // TODO: call naive
    //res = intersect::improved(w, q, params);
  };

  ed_string_w = "{GT,}{A,C}{T}{A,G}{ATGCGCTT}{TG,}{TGTTG}{GC,}{GCGGTGGCTT}{AC,G}";
  ed_string_q = "{AT,}{GAT}{C,G}{C}{G,T}{C}{AGG,TT}{T}{G,T}{TGTTGGCGCGGTGGCTT}{AC,G}";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w = "{A}{G,}{G}";
  ed_string_q = "{AGG}{C,}{G}";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "A{G,}{GGGCT,T}{A,T}";
  ed_string_q = "AGGG{C,}{G,}T";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "{AT,TC}{TC,T}";
  ed_string_q = "TC{,G}{CT,T}";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w = "{AT,TC}{ATC,}{T}";
  ed_string_q = "{TC,G}{CT,T}";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w = "A";
  ed_string_q = "{C,}A";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w = "A{C,}";
  ed_string_q = "{C,}";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "{C,}";
  ed_string_q = "{C,}A";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "A{C,}G";
  ed_string_q = "{C,}G";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "{C,}";
  ed_string_q = "{C,}A";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "{AT,TC}";
  ed_string_q = "{,G}AT";
  setup_and_run();
  EXPECT_EQ(res, true);

  // ed_string_w = "{GAATATGGCATTC}{A,C}{GTAATCCCT}{CG,}{TCGATGATC}";
  // ed_string_q = "{GAATATGGCATTCAGTAATCCCTTC}{GGCCG,}{GATGATC}";
  // ed_string_w = "{AT}{C,}{GA}";
  // ed_string_q = "{ATG}{C,}{A}";
  ed_string_w = "{GT,}{A,C}{T}";
  ed_string_q = "{AT,}{G}{T}";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "{GCGGTGGCTT}{AC,G}";
  ed_string_q = "{G,T}{TGTTGGCGCGGTGGCTT}{AC,G}";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "{AT,TC}{ATC,A}";
  ed_string_q = "{,G}{CT,T}";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "{AT,TC}{ATC,}";
  ed_string_q = "{TC,G}{CT,T}";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "{AT,TC}{ATC,T}";
  ed_string_q = "{,G}AT{CT,T}";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w = "{GT,}{A,C}{T}{A,G}{ATGCGCTT}{TG,}{TGTTG}{GC,}{GCGGTGGCTT}{AC,G}";
  ed_string_q = "{AT,}{GAT}{C,G}{C}{G,T}{C}{AGG,TT}{T}{G,T}{TGTTGGCGCGGTGGCTT}{AC,G}";
  setup_and_run();
  EXPECT_EQ(res, true);

  // ed_string_w = "{GT,}{A,C}{T}";
  // ed_string_q = "{G,}{AT}";
  // ed_string_w = "{GT,}{A,C}{T}{A,G}{ATG}";
  // ed_string_q = "{G,}{AT}{CC,GA}{T}{C,G}";
  ed_string_w = "{GT,}{A,C}{T}{A,G}{ATGCGCTT}";
  ed_string_q = "{G,}{AT}{CC,GA}{T}{C,G}{A,C}{G}{C,G}{TT}";
  setup_and_run();
  EXPECT_EQ(res, true);

  // ed_string_w = "{GCG}{TTA,}{TC}{G,T}";
  // ed_string_w = "{GCG}{TTA,}";
  // ed_string_q = "{GCGTT}";
  ed_string_w = "{GCG}{TTA,}{GTC}{G,T}";
  ed_string_q = "{GCGTT}{AG,T}{T}{AC,CG}";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w = "{AT,TC}{CGA,}{AGC,ATGC,}{ATC,T}";
  ed_string_q = "{TC,G}{CT,T}";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w = "{G,}{T,}{TTTACCC}{A,}{G}{A,G}";
  ed_string_q = "{T,}{TTTACCC}{AG,GA}{T}{G,T}{CAGG}";
  setup_and_run();
  EXPECT_EQ(res, false); //

  ed_string_w = "{AAC}{CAC,GGT}{TTTTCCATGAC}{A,}{AGGA}{G,}{TTGAATATGGCATTC}{A,C}";
  ed_string_q = "{AACCACTTTTCCATGAC}{A,}{AGGA}{G,}{TT}{A,}{"
    "GAATATGGCATTCAGTAATCCCTTC}{GGCCG,}";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "{AGGA}{G,}{TTGAATATGGCATTC}{A,C}{GTAATCCCT}{CG,}{TCGATGATC}";
  ed_string_q = "{AGGA}{G,}{TT}{A,}{GAATATGGCATTCAGTAATCCCTTC}";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "{AGGA}{G,}{TTGAATATGGCATTC}{A,C}{GTAATCCCT}{CG,}{TCGATGATC}";
  ed_string_q =
      "{AGGA}{G,}{TT}{A,}{GAATATGGCATTCAGTAATCCC}{T,}{T}{CGG,T}{C}{CG,}"
      "{GATGATCGCAGGGAGCGTT}";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "{GATGATC}{G,}{CA}{AGC,}{GGGAGCG}{TTA,}{G}";
  ed_string_q = "{GATGATCGCAGGGAGCGTT}{AG,T}";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w = "{AGGA}{G,}{TTGAATATGGCATTC}{A,C}";
  ed_string_q = "{AGGA}{G,}{TT}{A,}{GAATATGGCATTCA}";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w = "{T,}{GC}{G,}{GT}{CTT,GGC}";
  ed_string_q = "{T}{G,}{GCGTCTT}";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w = "{GC}{G,}{GT}{CTT,GGC}";
  ed_string_q = "{GCGTCTT}";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w = "{ATTCAGTCT}{G,}{TG}{C,}{GCCGC}{A,}{AACAA}{T,}{GC}{G,}{GT}{CTT,GGC}";
  ed_string_q = "{ATTCAGTCTGTGGCCGCAACAA}{GCG,}{T}{G,}{GCGTCTT}";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w =
      "{TGCAGGGGCTAATCGACCTCTGGC}{A,}{AAC}{CAC,GGT}{TTTTCCATGAC}{A,}{AGGA}{G,}{"
      "TTGAATATGGCATTC}{A,C}{GTAATCCCT}{CG,}{TCGATGATC}{G,}{CA}{AGC,}{GGGAGCG}{"
      "TTA,}{GTC}{G,T}{A}{AG,}{TATTGCG}{C,}{CA}{C,}{A,T}{ATGCGCAGG}{AGC,}{G}{"
      "TA,}{ATTCAGTCT}{G,}{TG}{C,}{GCCGC}{A,}{AACAA}{T,}{GC}{G,}{GT}{CTT,GGC}{"
      "GT}{A,}{C}{G,T}{C,T}{A,T}{CCGGCA}{G,}{GGCTGGGACAT}{TG,}{TGTGTC}{A,G}{A,"
      "G}{CCGCAG}{C,}{T}{C,T}{C,T}";
  ed_string_q =
    "{TGCAGGGGCTAATCGACCTCTGGC}{A,}{AACCACTTTTCCATGAC}{A,}{AGGA}{G,}"
    "{TT}{A,}{GAATATGGCATTCAGTAATCCCTTC}{GGCCG,}{GATGATCGCAGGGAGCGTT}{AG,T}{T}"
    "{AC,CG}{ATATTGCG}{CCA,}{CAATGCGCAGG}{A,G}{GC}{GT,}{AATTCAGTCTGTGGCCGCAACAA}"
    "{GCG,}{T}{G,}{GCGTCTTACCGGCA}{G,}{GGCTGGGACATTGTGTGTC}{AG,GA}{CCGCAG}"
    "{CTTT,TCAC}";
  setup_and_run();
  EXPECT_EQ(res, true);
}


TEST(IntersectionTest, EpsilonStart) {
  eds::EDS w, q;
  std::string ed_string_w, ed_string_q;
  bool res; // result
  core::AppConfig params;
  params.set_verbosity(0); // TODO: remove

  auto setup_and_run = [&]() {
    w = eds::Parser::from_string(ed_string_w);
    q = eds::Parser::from_string(ed_string_q);

    res = intersect::improved::has_intersection(w, q);
    // TODO: call naive
    //res = intersect::improved(w, q, params);
  };


  ed_string_w = "{GT,}{A,C}{T}{A,G}{ATGCGCTT}";
  ed_string_q = "{G,}{AT}{CC,GA}{T}{C,G}{A,C}{G}{C,G}{TT}";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w = "{GT,}{A,C}{T}{A,G}{ATGCGCTT}{TG,}{TGTTG}{GC,}{GCGGTGGCTT}{AC,G}";
  ed_string_q = "{AT,}{GAT}{C,G}{C}{G,T}{C}{AGG,TT}{T}{G,T}{TGTTGGCGCGGTGGCTT}{AC,G}";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w = "{GT,}{A,C}{T}{A,G}{ATGCGCTT}{TG,}{TGTTG}{GC,}{GCGGTGGCTT}{AC,G}";
  ed_string_q = "{AT,}{GAT}{C,G}{C}{G,T}{C}{AGG,TT}{T}{G,T}{TGTTGGCGCGGTGGCTT}{AC,G}";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w = "{G,}{T,}{TTTACCC}{A,}{G}{A,G}";
  ed_string_q = "{T,}{TTTACCC}{AG,GA}{T}{G,T}{CAGG}";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "A{C,}";
  ed_string_q = "{C,}";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "{C,}";
  ed_string_q = "{C,}A";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "A{C,}G";
  ed_string_q = "{C,}G";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "{C,}";
  ed_string_q = "{C,}A";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "{AT,TC}";
  ed_string_q = "{,G}AT";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w = "{GT,}{A,C}{T}";
  ed_string_q = "{AT,}{G}{T}";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "{AT,TC}{ATC,A}";
  ed_string_q = "{,G}{CT,T}";
  setup_and_run();
  EXPECT_EQ(res, false);

  ed_string_w = "{AT,TC}{ATC,T}";
  ed_string_q = "{,G}AT{CT,T}";
  setup_and_run();
  EXPECT_EQ(res, true);

  ed_string_w = "{T,}{GC}{G,}{GT}{CTT,GGC}";
  ed_string_q = "{T}{G,}{GCGTCTT}";
  setup_and_run();
  EXPECT_EQ(res, true);


}
