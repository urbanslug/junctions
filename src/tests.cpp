#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>

#include "./argvparser.hpp"
#include "./compute_intersect.cpp"
#include "./parseCmdArgs.hpp"
#include "graph.cpp"
#include "core.hpp"
#include "utils.hpp"

void test_string_matching();
void test_lacks_intersect();
void test_contains_intersect();
void test_parse_ed_string();
void test_handle_epsilon();
void test_contains_intersect_active_prefixes();
void test_compute_graph();
void test_msa_to_eds();

void print_properties(EDS &eds_w, EDS &eds_q) {
  std::cerr << " N: " << eds_w.size << " n: " << eds_w.length
            << " M: " << eds_q.size << " m: " << eds_q.length
            << std::endl;
}

int main(){

  // test_handle_epsilon();
  // test_contains_intersect();
  // test_lacks_intersect();
  // test_contains_intersect_active_prefixes();

  // test_parse_ed_string();

  test_string_matching();
  // test_compute_graph();

  /*
    test_lacks_intersect();
    test_contains_intersect();
    test_parse_ed_string();
    test_handle_epsilon();
    test_msa_to_eds();
  */
}

/*
  Tests
  -----
*/

// If parameter is not true, test fails
// This check function would be provided by the test framework
#define IS_TRUE(x) { if (!(x)) std::cerr << __FUNCTION__ << " failed on line " << __LINE__ << std::endl;}

#define IS_TRUE2(x, y) { if (!(x)) std::cerr << __FUNCTION__ << " failed test " << y << std::endl; }

core::Parameters init_tests() {
  core::Parameters test_params;
  test_params.verbosity = 5;

  return test_params;
};

/* -> */
bool logical_implication(bool p, bool q) { return !p || q; }

/* <-> */
bool double_implication(bool a, bool b) { return !(a && !b) && !(!a && b); }

void test_string_matching() {

  //std::string text, query;
  std::string text, query;

  // actual and expected query results
  vector<junctions::extended_match> match_positions, exp;

  // junctions::query_result exp; // expected

  std::vector<std::string> txt_strs;
  std::vector<slicex> text_slices;
  STvertex *root;

  auto print_match_positions =
      [&]() {
        std::cerr << "Query = " << query << std::endl
                  << "Text = " << text << std::endl
                  << "Matches found: " << match_positions.size() << std::endl;
        for (auto m : match_positions) { std::cerr << m << std::endl; }
      };

  auto setup = [&]() {
    junctions::join(txt_strs, '$', text);
    text += '_'; // add a terminator char
    root = Create_suffix_tree(text.c_str(), text.length());
    update_leaves(root, &text_slices);
  };

  txt_strs = {"ABC", "DAGT"};
  text_slices = {slicex{.start = 0, .length = 3},
                 slicex{.start = 3, .length = 4}};
  setup();

  query = "AB";
  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());

  exp = {
    junctions::extended_match{.beyond_text = false, .match_length = 2, .str_idx = 0, .chr_idx = 0}
  };

  IS_TRUE(exp == match_positions);

  // ---------------------

  query = "jk";
  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  // print_match_positions();
  exp = {};


  IS_TRUE(exp == match_positions);

  // ---------------------

  query = "A";
  // reset();
  // setup();

  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  // print_match_positions(match_positions);

  exp = {
      junctions::extended_match{.beyond_text = false, .match_length = 1, .str_idx = 0, .chr_idx = 0},
      junctions::extended_match{.beyond_text = false, .match_length = 1, .str_idx = 1, .chr_idx = 4}
  };


  IS_TRUE(exp == match_positions);

  // ---------------------

  // TODO: investigate

  query = "ABCDAGTA";

  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());

  //print_match_positions();

  exp = {
    junctions::extended_match{.beyond_text = true, .match_length = 3, .str_idx = 0, .chr_idx = 0}
  };


  IS_TRUE(exp == match_positions);

  // ---------------------

  query = "GTA";
  //reset();
  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  //print_match_positions();

  exp = {
    junctions::extended_match{.beyond_text = true, .match_length = 2, .str_idx = 1, .chr_idx = 5}
  };

  IS_TRUE(exp == match_positions);

  // ---------------------

  query = "GT";

  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  // print_match_positions();

  exp = {
    junctions::extended_match{.beyond_text = false, .match_length = 2, .str_idx = 1, .chr_idx = 5}
  };



  IS_TRUE(exp == match_positions);

  // ---------------------

  txt_strs = {"TG", "AT", "A"};

  text_slices = {
    slicex{.start = 0, .length = 2},
    slicex{.start = 2, .length = 2},
    slicex{.start = 4, .length = 1}};
  setup();

  query = "G";

  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  // print_match_positions();

  exp = {
    junctions::extended_match{.beyond_text = false, .match_length = 1, .str_idx = 0, .chr_idx = 1}
  };


  IS_TRUE(exp == match_positions);


  // ---------------------

  txt_strs = {"TCCCTTC"};
  text_slices = {slicex{.start = 0, .length = 7}};
  setup();

  query = "TCG";

  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  // print_match_positions();

  exp = {
    junctions::extended_match{.beyond_text = true, .match_length = 2, .str_idx = 0, .chr_idx = 5}
  };


  IS_TRUE(exp == match_positions);

  // ---------------------

  txt_strs = {"TCC"};
  text_slices = {slicex{.start = 0, .length = 3}};
  setup();

  query = "CA";
  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());


  // print_match_positions(match_positions);

  exp = {
    junctions::extended_match{.beyond_text = true, .match_length = 1, .str_idx = 0, .chr_idx = 2}
  };

  IS_TRUE(exp == match_positions);


  // --------------------------
  // txt_strs = {"AGT_"};
  // query = "TAC";

  txt_strs = {"AGT"};
  text_slices = {slicex{.start = 0, .length = 3}};
  setup();

  query = "GTC";

  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  // print_match_positions();

  exp = {
    junctions::extended_match{ .beyond_text = true, .match_length = 2, .str_idx = 0, .chr_idx = 1}
  };


  IS_TRUE(exp == match_positions);

  // --------------------------

  txt_strs = {"AGGAG"};
  text_slices = {slicex{.start = 0, .length = 5}};
  setup();

  query = "GAGG";

  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  // print_match_positions();


  exp = {
      junctions::extended_match{ .beyond_text = true, .match_length = 3, .str_idx = 0, .chr_idx = 2},
      junctions::extended_match{ .beyond_text = true, .match_length = 1, .str_idx = 0, .chr_idx = 4},
  };

  IS_TRUE(exp == match_positions);

  // --------------------------

  txt_strs = {"TTGTTC"};
  text_slices = {slicex{.start = 0, .length = 6}};
  setup();

  query = "TT";

  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  // print_match_positions();

  exp = {
      junctions::extended_match{ .beyond_text = false, .match_length = 2, .str_idx = 0, .chr_idx = 0},
      junctions::extended_match{ .beyond_text = false, .match_length = 2, .str_idx = 0, .chr_idx = 3},
  };

  IS_TRUE(exp == match_positions);

  // --------------------------
  txt_strs = {"TGTC"};
  text_slices = {slicex{.start = 0, .length = 4}};
  setup();

  query = "T";
  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  // print_match_positions();

  exp = {
      junctions::extended_match{.beyond_text = false, .match_length = 1, .str_idx = 0, .chr_idx = 0},
      junctions::extended_match{.beyond_text = false, .match_length = 1, .str_idx = 0, .chr_idx = 2},
  };

  IS_TRUE(exp == match_positions);

  // --------------------------

  txt_strs = {"CCGC"};
  text_slices = {slicex{.start = 0, .length = 4}};
  setup();
  query = "CC";

  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  // print_match_positions(match_positions);

  exp = {
      junctions::extended_match{.beyond_text = true, .match_length = 1, .str_idx = 0, .chr_idx = 0},
      junctions::extended_match{.beyond_text = false, .match_length = 2, .str_idx = 0, .chr_idx = 3},
  };

  IS_TRUE(exp == match_positions);


  // --------------------------

  txt_strs = {"TGAATGCCT"};
  text_slices = {slicex{.start = 0, .length = 9}};
  setup();

  query = "TG";

  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  // print_match_positions(match_positions);
  // print_match_positions();

  exp = {
      junctions::extended_match{.beyond_text = false, .match_length = 2, .str_idx = 0, .chr_idx = 0},
      junctions::extended_match{.beyond_text = false, .match_length = 2, .str_idx = 0, .chr_idx = 4},
      junctions::extended_match{.beyond_text = true, .match_length = 1, .str_idx = 0, .chr_idx = 8},
  };

  IS_TRUE(exp == match_positions);

  // --------------------------

  txt_strs = {"TGATGCT"};
  text_slices = {slicex{.start = 0, .length = 7}};
  setup();

  query = "TG";

  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  // print_match_positions(match_positions);

  exp = {
      junctions::extended_match{.beyond_text = false, .match_length = 2, .str_idx = 0, .chr_idx = 0},
      junctions::extended_match{.beyond_text = false, .match_length = 2, .str_idx = 0, .chr_idx = 3},
      junctions::extended_match{.beyond_text = true, .match_length = 1, .str_idx = 0, .chr_idx = 6},
  };

  IS_TRUE(exp == match_positions);

  // --------------------------

  txt_strs = {"TCG", "CGC"};
  text_slices = {slicex{.start = 0, .length = 3},
                 slicex{.start = 3, .length = 3}};

  setup();
  query = "GAACA";

  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  // print_match_positions(match_positions);


  exp = {
    junctions::extended_match{ .beyond_text = true, .match_length = 1, .str_idx = 0, .chr_idx = 2}
  };

  IS_TRUE(exp == match_positions);

  // --------------------------

  query = "G";

  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  // print_match_positions(match_positions);

  exp = {
      junctions::extended_match{.beyond_text = false, .match_length = 1, .str_idx = 0, .chr_idx = 2},
      junctions::extended_match{.beyond_text = false, .match_length = 1, .str_idx = 1, .chr_idx = 4},
  };

  IS_TRUE(exp == match_positions);

  // --------------------------

  txt_strs = {"CT"
              "TGGATACAGAGCCACGGT"
              "T"};
  text_slices = {slicex{.start = 0, .length = 110}};

  // TODO: finish test

  // --------------------------

  txt_strs = {"G", "C"};
  text_slices = {slicex{.start = 0, .length = 1},
                 slicex{.start = 1, .length = 1}};

  setup();

  query = "TAC";
  // query = "TGCTACGCGTGGG";

  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  // print_match_positions(match_positions);

  // --------------------------

  txt_strs = {"CTCCCAAGACCTAAGCGCTGAGGCACATATACGCGTACACCTAGCATTTTGTTAGGGAGCAG"
              "TGGATACAGAGCCACGGT"
              "TCTTACTGGGGGGCAGTCAACTACAC"
              "CTA"
              "T"};
  text_slices = {slicex{.start = 0, .length = 110}};

  setup();

  query = "TGC";
  // query = "TGCTACGCGTGGG";

  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  // print_match_positions(match_positions);

  // --------------------------

  txt_strs = {"TTGTGCAATGGTTTAAAAAGTCCAACATGGATACCGCTTGGGGAGGCTGAGACGATGGTTGTGA"
              "CAGGGCCTCTACACCTAT"};
  text_slices = {slicex{.start = 0, .length = 82}};

  setup();

  query = "TGCTACGCGTGGGCAGTGTTGTTAA";

  match_positions = FindEndIndexesThree(query.c_str(), root, text.c_str());
  // print_match_positions(match_positions);

  // --------------------------

  txt_strs = {"GAT",
              "GGG",
              "GCG"};
  text_slices = {
      slicex{.start = 0, .length = 3},
      slicex{.start = 3, .length = 3},
      slicex{.start = 6, .length = 3},
  };

  junctions::join(txt_strs, '$', text);
  text += '_'; // add a terminator char
  root = Create_suffix_tree(text.c_str(), text.length());
  update_leaves(root, &text_slices);

  query = "GAT";
  vector<junctions::extended_match> k = FindEndIndexesThree(query.c_str(), root, text.c_str());
}

void test_compute_graph() {
  auto params = init_tests();
  std::string ed_string_w, ed_string_q;
  EDS eds_w, eds_q;

  graph::Graph g(0,0);
  bool contains_intersect;
  int len;

  auto check = [&](int line) {
    eds_w = parser::parse_ed_string(ed_string_w, params);
    eds_q = parser::parse_ed_string(ed_string_q, params);

    g = graph::compute_intersection_graph(eds_w, eds_q, params);
    g.print_dot();
    contains_intersect = improved::intersect(eds_w, eds_q, params) && naive::intersect(eds_w, eds_q, params);
    len = graph::longest_witness(g);
    IS_TRUE2(double_implication(len >= 0, contains_intersect), line);
  };

  ed_string_w = "TTT"
                "{A,}"
                "TTG"
                "TGCAATGGTTTAAAAAGTCCAACATGGATACCGCTTGGGGAGGCT"
                "GAGA"
                "CG"
                "ATGGTTGTGACAG"
                "GGC"
                "CTCTACAC"
                "CTA"
                "T"
                "{G,C,}"
                "CTA"
                "CGCGTGGGCAGTGTTGTTAA";
  ed_string_q = "TTT"
                "A"
                "TTG"
                "TGCAATGGTTTAAAAAGTCCAACATGGATACCGCTTGGGGAGGCT"
                "GAGA"
                "{CG,AT,GC}"
                "ATGGTTGTGACAG"
                "{GGC,T,CCA}"
                "CTCTACAC"
                "{CTA,GCT,TAC}"
                "T"
                "G"
                "CTA"
                "CGCGTGGGCAGTGTTGTTAA";
  check(__LINE__);
  // return;

  /*
    active suffixes
    ---------------
  */

  ed_string_w = "CTCCCAAGACCTAAGCGCTGAGGCACATATACGCGTACACCTAGCATTTTGTTAGGGAGCAG"
                "TGGATACAGAGCCACGGT"
                "TCTTACTGGGGGGCAGTCAACTACAC"
                "CTA"
                "T"
                "{G,C,}"
                "CTACGCGTGGG";

  ed_string_q = "CTCCCAAGACCTAAGCGCTGAGGCACATATACGCGTACACCTAGCATTTTGTTAGGGAGCAG"
                "TGGATACAGAGCCACGGT"
                "TCTTACTGGGGGGCAGTCAACTACAC"
                "{CTA,GCT,TAC}"
                "T"
                "G"
                "CTACGCGTGGG";
  check(__LINE__);

  ed_string_w = "{GCG}{TTA,}{GTC}{G,T}";
  ed_string_q = "{GCGTT}{AG,T}{T}{AC,CG}";
  check(__LINE__);

  ed_string_w = "TCTC{TCG,CGC}AACAATCACTCAA";
  ed_string_q = "TCTC{TC,CG}GAACA{A,,C}TCAC{TC,CT,A}AA";
  check(__LINE__);
  //return;

  ed_string_w = "{CCG}{CC,G}{AGA}";
  ed_string_q = "{CCGC}{C,GAGG}{AGA}";
  check(__LINE__);


  ed_string_w = "{CAGG}{A,}{GAGGCACAATGAGCCAGCA}";
  ed_string_q = "{CAGGAG}{A,}{GGCACAATGAGCCAGCA}";
  check(__LINE__);

  ed_string_w = "{AGG}{A,}{GAGG}";
  ed_string_q = "{AGGAG}{A,}{GG}";
  check(__LINE__);

  ed_string_w = "{TTC}{A,GC}{CATT}{AC,}{ACTGGCCCGCGCGGTGG}";
  ed_string_q = "{TTCAC}{AT,C}{C,T}{AC}{T,}{TG}{A,}{GCCCGCGCGGTGGT}";
  check(__LINE__);

  ed_string_w = "{TTC}{A,GC}{CATT}{AC,}{ACTGGCCC}";
  ed_string_q = "{TTCAC}{AT,C}{C,T}{AC}{T,}{TG}{A,}{GCCC}";
  check(__LINE__);

  ed_string_w = "AATCAGTGCTTCTAGCTCTTGGAGGGCTTGTACATTAACGGAACT{G,C}CA";
  ed_string_q = "AATCAGTGCTTCTAGCTCTTGGAGGGCTTGTACATTAACGGAAC{TG,AT,A}CA";
  check(__LINE__);

  ed_string_w = "GTGACATGTACATTACT{G,C}CA";
  ed_string_q = "GTGACATGTACATTAC{TG,AT,A}CA";
  check(__LINE__);

  /*
    full matches
    ------------
  */

  ed_string_w = "{AT,TC}";
  ed_string_q = "{,G}";
  check(__LINE__);

  ed_string_w = "{AT,TC}";
  ed_string_q = "{TC,G}";
  check(__LINE__);

  ed_string_w = "{ACT}";
  ed_string_q = "{A}{C}{T}";
  check(__LINE__);

  ed_string_w = "{AT,TC}{AC,T}";
  ed_string_q = "{TC,G}{CT,T}";
  check(__LINE__);

  // active suffixes
  ed_string_w = "{AT,TC}{ATC,T}";
  ed_string_q = "{TC,G}{CT,T}";
  check(__LINE__);

  /*
    epsilons
    --------
   */

  ed_string_w = "{T}{C}";
  ed_string_q = "T{,G}{C}";
  check(__LINE__);

  ed_string_w = "{AT,TC}{ATC,T}";
  ed_string_q = "TC{,G}{CT,T}";

  check(__LINE__);

  ed_string_w = "{GT,}{A,C}{T}{A,G}{AT}";
  ed_string_q = "{AT,}{GAT}";
  check(__LINE__);

  ed_string_w = "{AT,TC}{ATC,T}";
  ed_string_q = "{,G}AT{CT,T}";
  check(__LINE__);

  ed_string_w = "{TTGA}";
  ed_string_q = "{TT}{A,}{GA}";
  check(__LINE__);

  ed_string_w = "{T}";
  ed_string_q = "{T}{A,}";
  check(__LINE__);

  ed_string_w = "{TG}";
  ed_string_q = "{T}{A,}{G}";
  check(__LINE__);

  ed_string_w = "{TTGAATATGGCATTC}";
  ed_string_q = "{TT}{A,}{GAATATGGCATTC}";
  check(__LINE__);

  ed_string_w = "{GT,}{A,C}{T}{A,G}{ATGCGCTT}{TG,}{TGTTG}{GC,}{GCGGTGGCTT}{AC,G}";
  ed_string_q = "{AT,}{GAT}{C,G}{C}{G,T}{C}{AGG,TT}{T}{G,T}{TGTTGGCGCGGTGGCTT}{AC,G}";
  check(__LINE__);

  ed_string_w =
      "{TGCAGGGGCTAATCGACCTCTGGC}{A,}{AACCACTTTTCCATGAC}{A,}{AGGA}"
      "{G,}{TT}{A,}{GAATATGGCATTCAGTAATCCCTTC}{GGCCG,}{GATGATCGCAGGGAGCGTT}";
  ed_string_q = "{TGCAGGGGCTAATCGACCTCTGGC}{A,}{AAC}{CAC,GGT}{TTTTCCATGAC}{A,}{"
                "AGGA}{G,}{TTGAATATGGCATTC}{A,C}{GTAATCCCT}{CG,}{TCGATGATC}{G,}"
                "{CA}{AGC,}{GGGAGCG}{TT,}";
  check(__LINE__);


  ed_string_w = "{TCCCTTC}{GGCCG,}{G}";
  ed_string_q = "{TCCCT}{CG,}{TCG}";
  check(__LINE__);

  ed_string_w = "{GTAATCCCTTC}{GGCCG,}{GATGATC}";
  ed_string_q = "{GTAATCCCT}{CG,}{TCGATGATC}";
  check(__LINE__);

  ed_string_w = "{TC}{G,}{G}";
  ed_string_q = "{T}{CG,}{TCG}";
  // TODO: figure out this test
  check(__LINE__);

  ed_string_w = "C{TG,AT,A}C";
  ed_string_q = "CT{G,C}C";
  check(__LINE__);

  ed_string_w = "C{TG,AT,A}C";
  ed_string_q = "CT{G,C}C";
  check(__LINE__);

  ed_string_w = "{GT,}{A,C}{T}{A,G}{ATGCGCTT}";
  ed_string_q = "{G,}{AT}{CC,GA}{T}{C,G}{A,C}{G}{C,G}{TT}";
  check(__LINE__);

  ed_string_w = "{GCGGTGGCTT}{AC,G}";
  ed_string_q = "{G,T}{TGTTGGCGCGGTGGCTT}{AC,G}";
  check(__LINE__);

  ed_string_w = "{GCG}{TTA,}{GTC}{G,T}";
  ed_string_q = "{GCGTT}{AG,T}{T}{AC,CG}";
  check(__LINE__);

  ed_string_w = "{AGGA}{G,}{TTGAATATGGCATTC}{A,C}";
  ed_string_q = "{AGGA}{G,}{TT}{A,}{GAATATGGCATTCA}";
  check(__LINE__);

  ed_string_w = "{AGGA}{G,}{TTGAATATGGCATTC}{A,C}{GTAATCCCT}{CG,}{TCGATGATC}";
  ed_string_q = "{AGGA}{G,}{TT}{A,}{GAATATGGCATTCAGTAATCCCTTC}";
  check(__LINE__);

  ed_string_w = "{A,G}{CTC,CAAGT}{GA,CTA}";
  ed_string_q = "{ACTCGA}";
  check(__LINE__);

  ed_string_w = "CT{G,C}CACC";
  ed_string_q = "C{TG,AT,A}CACC";
  check(__LINE__);


}

void test_msa_to_eds() {
  string_vec msa;
  auto params = init_tests();

  msa = {
    "ACGTGTACA-GTTGAC",
    "A-G-GTACACGTT-AC",
    "A-GTGT-CACGTTGAC",
    "ACGTGTACA--TTGAC"
  };
  parser::msa_to_eds(msa);

  msa = {
    "CGTGTACA-GTTGACG",
    "-G-GTACACGTT-ACC",
    "-GTGTTCACGTTGACC",
    "CGTGTACA--TTGACC"
  };
  parser::msa_to_eds(msa);

  msa = {
    "GT-C-T--AAT-G--C-G-CTT",
    "---A-T--GAT-G--C-G-CTT",
  };
  // std::string ed_string = parser::msa_to_eds(msa);
  // EDS e = parser::parse_ed_string(ed_string, params);
  // utils::print_edt(e);
  parser::msa_to_eds(msa);

  msa = {
    "---A-T--GAT-G--C-G-CTT",
    "--GA-T--CCT-C--A-G-GTT"
  };
  //ed_string = parser::msa_to_eds(msa);
  // e = parser::parse_ed_string(ed_string, params);
  // utils::print_edt(e);
  parser::msa_to_eds(msa);
}

void test_parse_ed_string() {
  std::string ed_string;
  EDS eds;
  auto params = init_tests();
  // TODO: confirm size, len, offsets etc for all

  ed_string = "{AT,TC}{ATC,T}";
  eds = parser::parse_ed_string(ed_string, params);
  // print_edt(eds);

  ed_string = "{AT,TC}{ATC,}CTA";
  eds = parser::parse_ed_string(ed_string, params);

  ed_string = "ACTGAC{AT,,TC}AGG{,ATC,}CT{AT,TC}A";
  // ed_string = "ACC{AT,,TC}AGG";
  eds = parser::parse_ed_string(ed_string, params);
  // print_edt(eds);

  ed_string = "ACTGACCT";
  eds = parser::parse_ed_string(ed_string, params);
  // print_edt(eds);

  ed_string = "{AT,TC}{ATC,T}";
  eds = parser::parse_ed_string(ed_string, params);
  // print_edt(eds);

  ed_string = "{,G}{CT,T}";
  eds = parser::parse_ed_string(ed_string, params);
  IS_TRUE(eds.data[0].has_epsilon);
  IS_TRUE(!eds.data[1].has_epsilon);
  IS_TRUE(eds.length == 2);
  // print_edt(eds);

  std::vector<std::vector<span>> str_offsets = eds.str_offsets;
}

void test_contains_intersect() {
  auto params = init_tests();

  std::string ed_string_w = "{AT,TC}{ATC,T}";
  std::string ed_string_q = "{TC,G}{CT,T}";

  EDS eds_w = parser::parse_ed_string(ed_string_w, params);
  EDS eds_q = parser::parse_ed_string(ed_string_q, params);

  // IS_TRUE(improved::intersect(eds_w, eds_q));
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));
}

void test_contains_intersect_active_prefixes() {
  std::string ed_string_w, ed_string_q;
  EDS eds_w, eds_q;

  auto params = init_tests();

  auto run_n_print = [&]() {
    bool naive_res = naive::intersect(eds_w, eds_q, params);
    bool imp_res = improved::intersect(eds_w, eds_q, params);
    std::cerr << "naive: " << naive_res << " imp: " << imp_res;
    exit(1);
  };

  ed_string_w = "{AT,TCT}";
  ed_string_q = "{TC,G}{CT,T}";

  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);

  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));

  ed_string_w = "{A,G}{CTC,CAAGT}{GA,CTA}";
  ed_string_q = "{ACTCGA}";


  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);

  utils::print_edt(eds_w);
  utils::print_edt(eds_q);

  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));

  // exit(1);

  ed_string_w = "{A,G}{CTC,CAAGT}{GA,CTA}{TC,G}{CT,T}";
  ed_string_q = "{ACTCGA}{AT,TCT}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  // utils::print_edt(eds_w);
  // utils::print_edt(eds_q);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));

  ed_string_w = "{ACTCGA}{AT,TCT}";
  ed_string_q = "{A,G}{CTC,CAAGT}{GA,CTA}{TC,G}{CT,T}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  // utils::print_edt(eds_w);
  // utils::print_edt(eds_q);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));

}

void test_lacks_intersect() {
  std::string ed_string_w, ed_string_q;
  EDS eds_w, eds_q;

  auto params = init_tests();

  auto run_n_print = [&]() {
    bool naive_res = naive::intersect(eds_w, eds_q, params);
    bool imp_res = improved::intersect(eds_w, eds_q, params);
    std::cerr << "naive: " << naive_res << " imp: " << imp_res;
    exit(1);
  };

  ed_string_w = "{GC}{AC,G}";
  ed_string_q = "{G,T}{GC}{AC,G}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  // run_n_print();
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));

  ed_string_w = "{GCGGTGGCTT}{AC,G}";
  ed_string_q = "{G,T}{GCGGTGGCTT}{AC,G}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));

  ed_string_w = "{AT,TC}{ATC,T}";
  ed_string_q = "{TC,G}{CG,G}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));
}

void test_handle_epsilon() {
  std::string ed_string_w, ed_string_q;
  EDS eds_w, eds_q;

  auto params = init_tests();

  auto run_n_print =[&](){
    bool naive_res = naive::intersect(eds_w, eds_q, params);
    bool imp_res = improved::intersect(eds_w, eds_q, params);
    std::cerr << "naive: " << naive_res << " improved: " << imp_res;
    exit(1);
  };

  ed_string_w = "{AT,TC}{TC,T}";
  ed_string_q = "TC{,G}{CT,T}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  // run_n_print();
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));
  // exit(1);

  ed_string_w = "{AT,TC}{ATC,}{T}";
  ed_string_q = "{TC,G}{CT,T}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));

  ed_string_w = "A";
  ed_string_q = "{C,}A";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);

  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));

  // ed_string_w = "A{C,}G";
  // ed_string_q = "{C,}G";
  ed_string_w = "A{C,}";
  ed_string_q = "{C,}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  // run_n_print();
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));

  ed_string_w = "{C,}";
  ed_string_q = "{C,}A";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  // run_n_print();
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));

  ed_string_w = "{AT,TC}";
  ed_string_q = "{,G}AT";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));

  // ed_string_w = "{GAATATGGCATTC}{A,C}{GTAATCCCT}{CG,}{TCGATGATC}";
  // ed_string_q = "{GAATATGGCATTCAGTAATCCCTTC}{GGCCG,}{GATGATC}";
  // ed_string_w = "{AT}{C,}{GA}";
  // ed_string_q = "{ATG}{C,}{A}";
  ed_string_w = "{GT,}{A,C}{T}";
  ed_string_q = "{AT,}{G}{T}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  // run_n_print();
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));



  ed_string_w = "{AT,TC}{ATC,T}";
  ed_string_q = "{,G}AT{CT,T}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  // run_n_print();
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));

  ed_string_w = "{GT,}{A,C}{T}{A,G}{ATGCGCTT}{TG,}{TGTTG}{GC,}{GCGGTGGCTT}{AC,G}";
  ed_string_q = "{AT,}{GAT}{C,G}{C}{G,T}{C}{AGG,TT}{T}{G,T}{TGTTGGCGCGGTGGCTT}{AC,G}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
          improved::intersect(eds_w, eds_q, params));

  // ed_string_w = "{GT,}{A,C}{T}";
  // ed_string_q = "{G,}{AT}";
  // ed_string_w = "{GT,}{A,C}{T}{A,G}{ATG}";
  // ed_string_q = "{G,}{AT}{CC,GA}{T}{C,G}";
  ed_string_w = "{GT,}{A,C}{T}{A,G}{ATGCGCTT}";
  ed_string_q = "{G,}{AT}{CC,GA}{T}{C,G}{A,C}{G}{C,G}{TT}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  // run_n_print();
  IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
          improved::intersect(eds_w, eds_q, params));

  ed_string_w = "{GCG}{TTA,}{GTC}{G,T}";
  // ed_string_w = "{GCG}{TTA,}{TC}{G,T}";
  ed_string_q = "{GCGTT}{AG,T}{T}{AC,CG}";
  // ed_string_w = "{GCG}{TTA,}";
  // ed_string_q = "{GCGTT}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
          improved::intersect(eds_w, eds_q, params));


  // exit(1);

  ed_string_w = "{GCGGTGGCTT}{AC,G}";
  ed_string_q = "{G,T}{TGTTGGCGCGGTGGCTT}{AC,G}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
          improved::intersect(eds_w, eds_q, params));
  // exit(1);



  ed_string_w = "{AT,TC}{ATC,A}";
  ed_string_q = "{,G}{CT,T}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
          improved::intersect(eds_w, eds_q, params));

  ed_string_w = "{AT,TC}{ATC,}";
  ed_string_q = "{TC,G}{CT,T}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
          improved::intersect(eds_w, eds_q, params));



  ed_string_w = "{AT,TC}{CGA,}{AGC,ATGC,}{ATC,T}";
  ed_string_q = "{TC,G}{CT,T}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
          improved::intersect(eds_w, eds_q, params));

  ed_string_w = "{G,}{T,}{TTTACCC}{A,}{G}{A,G}";
  ed_string_q = "{T,}{TTTACCC}{AG,GA}{T}{G,T}{CAGG}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
          improved::intersect(eds_w, eds_q, params));

  ed_string_w = "{GATGATC}{G,}{CA}{AGC,}{GGGAGCG}{TTA,}{G}";
  ed_string_q = "{GATGATCGCAGGGAGCGTT}{AG,T}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
          improved::intersect(eds_w, eds_q, params));

  ed_string_w = "{AGGA}{G,}{TTGAATATGGCATTC}{A,C}";
  ed_string_q = "{AGGA}{G,}{TT}{A,}{GAATATGGCATTCA}";
  ed_string_w = "{AGGA}{G,}{TTGAATATGGCATTC}{A,C}{GTAATCCCT}{CG,}{TCGATGATC}";
  ed_string_q = "{AGGA}{G,}{TT}{A,}{GAATATGGCATTCAGTAATCCCTTC}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
          improved::intersect(eds_w, eds_q, params));

  ed_string_w = "{AGGA}{G,}{TTGAATATGGCATTC}{A,C}{GTAATCCCT}{CG,}{TCGATGATC}";
  ed_string_q =
      "{AGGA}{G,}{TT}{A,}{GAATATGGCATTCAGTAATCCC}{T,}{T}{CGG,T}{C}{CG,}"
      "{GATGATCGCAGGGAGCGTT}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
          improved::intersect(eds_w, eds_q, params));

  ed_string_w =
      "{AAC}{CAC,GGT}{TTTTCCATGAC}{A,}{AGGA}{G,}{TTGAATATGGCATTC}{A,C}";
  ed_string_q = "{AACCACTTTTCCATGAC}{A,}{AGGA}{G,}{TT}{A,}{"
                "GAATATGGCATTCAGTAATCCCTTC}{GGCCG,}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
          improved::intersect(eds_w, eds_q, params));

  ed_string_w =
      "{TGCAGGGGCTAATCGACCTCTGGC}{A,}{AAC}{CAC,GGT}{TTTTCCATGAC}{A,}{AGGA}{G,}{"
      "TTGAATATGGCATTC}{A,C}{GTAATCCCT}{CG,}{TCGATGATC}{G,}{CA}{AGC,}{GGGAGCG}{"
      "TTA,}{GTC}{G,T}{A}{AG,}{TATTGCG}{C,}{CA}{C,}{A,T}{ATGCGCAGG}{AGC,}{G}{"
      "TA,}{ATTCAGTCT}{G,}{TG}{C,}{GCCGC}{A,}{AACAA}{T,}{GC}{G,}{GT}{CTT,GGC}{"
      "GT}{A,}{C}{G,T}{C,T}{A,T}{CCGGCA}{G,}{GGCTGGGACAT}{TG,}{TGTGTC}{A,G}{A,"
      "G}{CCGCAG}{C,}{T}{C,T}{C,T}";


  ed_string_q = "{TGCAGGGGCTAATCGACCTCTGGC}{A,}{AACCACTTTTCCATGAC}{A,}{AGGA}{G,}{TT}{A,}{GAATATGGCATTCAGTAATCCCTTC}{GGCCG,}{GATGATCGCAGGGAGCGTT}{AG,T}{T}{AC,CG}{ATATTGCG}{CCA,}{CAATGCGCAGG}{A,G}{GC}{GT,}{AATTCAGTCTGTGGCCGCAACAA}{GCG,}{T}{G,}{GCGTCTTACCGGCA}{G,}{GGCTGGGACATTGTGTGTC}{AG,GA}{CCGCAG}{CTTT,TCAC}";


  // ed_string_w = "{CCGCAG}{C,}{T}{C,T}{C,T}";
//ed_string_q = "{CCGCAG}{CTTT,TCAC}";

// ed_string_w = "{C,}{T}{C,T}{C,T}";
// ed_string_q = "{CTTT,TCAC}";

eds_w = parser::parse_ed_string(ed_string_w, params);
eds_q = parser::parse_ed_string(ed_string_q, params);
// run_n_print();
IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
        improved::intersect(eds_w, eds_q, params));
// exit(1);

}
