#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>

#include "./argvparser.hpp"
#include "./compute_intersect.cpp"
#include "./parseCmdArgs.hpp"
#include "core.hpp"

void test_lacks_intersect();
void test_contains_intersect();
void test_parse_ed_string();
void test_handle_epsilon();
void test_msa_to_eds();

int main() {
  test_handle_epsilon();

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
#define IS_TRUE(x) { if (!(x)) std::cout << __FUNCTION__ << " failed on line " << __LINE__ << std::endl;}

core::Parameters init_tests() {
  core::Parameters test_params;
  test_params.verbosity = 5;

  return test_params;
};

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

  ed_string = "{AT,TC}{ATC,}";
  eds = parser::parse_ed_string(ed_string, params);
  // print_edt(eds);

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

void test_handle_epsilon() {
  std::string ed_string_w, ed_string_q;
  EDS eds_w, eds_q;

  auto params = init_tests();

  ed_string_w = "{A,GTC}{T}{A,G}{ATGCGCTT}{TG,}{TGTTG}{GC,}{GCGGTGGCTT}{AC,G}{A}{TC,}{TGCAGGGGCTAATCGACCTCTGGC}{A,}{AAC}{CAC,GGT}{TTTTCCATGAC}{A,}{AGGA}{G,}{TTGAATATGGCATTC}{A,C}{GTAATCCCT}{CG,}{TCGATGATC}{G,}{CA}{AGC,}{GGGAGCG}{TTA,}{GTC}{G,T}{A}{AG,}{TATTGCG}{C,}{CA}{CA,T}{ATGCGCAGG}{AGC,}{G}{TA,}{ATTCAGTCT}{G,}{TG}{C,}{GCCGC}{A,}{AACAA}{T,}{GC}{G,}{GT}{CTT,GGC}{GT}{A,}{C}{GCT,TTA}{CCGGCA}{G,}{GGCTGGGACAT}{TG,}{TGTGTC}{AG,GA}{CCGCAG}{C,}{T}{CC,TT}{ACAGGC}{G,}{GT}{CCT,TGG}{G}{C,T}{C}{GG,}{AGCAGTGGCT}{GGA,}{GGAAGCCCACT}{G,}{GG}{C,G}{GT}{CA,}{ACTCTGACACCGAC}{T,}{GAA}{GT,}{TTTACCC}{A,}{G}{A,G}{TT}{TG,}{CAGGAG}{A,}{GGCACAATGAGCCAGCA}{AG,}{TTTACCT}{C,}{G}{GCT,TTA}{T}{C,G}{TG}{C,}{CCGCACAGCC}{A,}{C}{A,}{GGCATCT}{C,GG}{AT}{C,}{G}{TAA,}{GCAGAAAAACTGTCAGAATTA}{C,}{C}{CTC,TAG}{CC}{C,G}{CT}{G,}{GC}{GGAG,TCGC}{C}{A,G}{TG}{G,T}{GCGCAT}{C,}{T}{A,CCT}{CGTTGAGTTAACCGC}{C,GAGG}{AGA}{G,}{GTTGA}{C,}{TTCAC}{ATT,CC}{AC}{T,}{TG}{A,}{GCCCGCGCGGTGGT}{CC,T}{GCC}{T,}{G}{T,}{AC}{T,}{AC}{A,}{G}{C,}{C}{A,C}{A}{CG,TA}{GCA}{G,}{GATAC}";

  ed_string_q =
      "{AT,}{GAT}{C,G}{C}{G,T}{C}{AGG,TT}{T}{G,T}{TGTTGGCGCGGTGGCTT}{AC,G}{A}{"
      "TC,}{TGCAGGGGCTAATCGACCTCTGGC}{A,}{AACCACTTTTCCATGAC}{A,}{AGGA}{G,}{TT}{"
      "A,}{GAATATGGCATTCAGTAATCCCTTC}{GGCCG,}{GATGATCGCAGGGAGCGTT}{AG,T}{T}{AC,"
      "CG}{ATATTGCG}{CCA,}{CAATGCGCAGG}{A,G}{GC}{GT,}{AATTCAGTCTGTGGCCGCAACAA}{"
      "GCG,}{T}{G,}{GCGTCTTACCGGCA}{G,}{GGCTGGGACATTGTGTGTC}{AG,GA}{CCGCAG}{"
      "CTTT,TCAC}{ACAGGC}{G,}{GT}{CCT,TGG}{G}{C,T}{C}{GG,}{"
      "AGCAGTGGCTGGAAGCCCACT}{G,}{GG}{C,G}{GT}{CA,}{ACTCTGACACCGAC}{T,}{GAA}{"
      "GT,}{TTTACCC}{AG,GA}{T}{G,T}{CAGG}{A,}{GAGGCACAATGAGCCAGCA}{AG,}{"
      "TTTACCT}{G,}{TT}{AT,G}{GT}{CG,GC}{CCGCACAGCC}{A,}{C}{A,}{GGCATCT}{G,}{"
      "GGATGGCAG}{A,}{AAAACTGTCAGAATTAC}{CCT,TAG}{CCGC}{CT,TGC}{GGAGC}{T,}{GT}{"
      "G,}{GGCGCAT}{C,}{T}{A,CCT}{CGTTGAGTTAACCG}{CC,G}{AGA}{G,}{GTTGA}{C,}{"
      "TTC}{A,GC}{CATT}{AC,}{ACTGGCCCGCGCGGTGG}{T,}{T}{AT,CC}{GCC}{G,}{GACTA}{"
      "G,}{CGCA}{AT,}{AGCAGATACGC}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));
  exit(1);

  ed_string_w = "{GCG}{TTA,}{GTC}{G,T}";
  ed_string_q = "{GCGTT}{AG,T}{T}{AC,CG}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));
  

  ed_string_w = "{GT,}{A,C}{T}{A,G}{ATGCGCTT}";
  ed_string_q = "{G,}{AT}{CC,GA}{T}{C,G}{A,C}{G}{C,G}{TT}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));

  ed_string_w = "{AT,TC}{TC,T}";
  ed_string_q = "TC{,G}{CT,T}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  // IS_TRUE(improved::intersect(eds_w, eds_q));
  /*
  std::cout << "naive: "     << naive::intersect(eds_w, eds_q)
            << " improved: " << improved::intersect(eds_w, eds_q)
            << std::endl;
            */
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));

  ed_string_w = "{AT,TC}{ATC,}";
  ed_string_q = "{TC,G}{CT,T}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  // IS_TRUE(!improved::intersect(eds_w, eds_q));
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));

  ed_string_w = "{AT,TC}{ATC,T}";
  ed_string_q = "{,G}AT{CT,T}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(improved::intersect(eds_w, eds_q, params));
  /*
  std::cout << "naive: "     << naive::intersect(eds_w, eds_q)
            << " improved: " << improved::intersect(eds_w, eds_q)
            << std::endl;
  */
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));

  ed_string_w = "{AT,TC}{ATC,A}";
  ed_string_q = "{,G}{CT,T}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  // IS_TRUE(!improved::intersect(eds_w, eds_q));
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));
  

  ed_string_w = "{AT,TC}{CGA,}{AGC,ATGC,}{ATC,T}";
  ed_string_q = "{TC,G}{CT,T}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  // improved::intersect(eds_w, eds_q);
  /*
  std::cout << "naive: "     << naive::intersect(eds_w, eds_q)
            << " improved: " << improved::intersect(eds_w, eds_q)
            << std::endl;
            */
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));
  
}

void test_lacks_intersect() {
  auto params = init_tests();

  std::string ed_string_w = "{AT,TC}{ATC,T}";
  std::string ed_string_q = "{TC,G}{CG,G}";
  EDS eds_w = parser::parse_ed_string(ed_string_w, params);
  EDS eds_q = parser::parse_ed_string(ed_string_q, params);
  improved::intersect(eds_w, eds_q, params);
  //IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));
}

