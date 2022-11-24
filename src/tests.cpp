
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
#include "core.hpp"

void test_lacks_intersect();
void test_contains_intersect();
void test_parse_ed_string();
void test_handle_epsilon();
void test_msa_to_eds();

void print_properties(EDS &eds_w, EDS &eds_q) {
  std::cerr << " N: " << eds_w.size << " n: " << eds_w.length
            << " M: " << eds_q.size << " m: " << eds_q.length
            << std::endl;
}

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

  auto run_n_print =[&](){
    bool naive_res = naive::intersect(eds_w, eds_q, params);
    bool imp_res = improved::intersect(eds_w, eds_q, params);
    std::cerr << "naive: " << naive_res << " imp: " << imp_res;
    exit(1);
  };


  // ed_string_w = "{GAATATGGCATTC}{A,C}{GTAATCCCT}{CG,}{TCGATGATC}";
  // ed_string_q = "{GAATATGGCATTCAGTAATCCCTTC}{GGCCG,}{GATGATC}";
  // ed_string_w = "{AT}{C,}{GA}";
  // ed_string_q = "{ATG}{C,}{A}";

  ed_string_w = "{TGCAGGGGCTAATCGACCTCTGGC}{A,}{AAC}{CAC,GGT}{TTTTCCATGAC}{A,}{AGGA}{G,}{TTGAATATGGCATTC}{A,C}{GTAATCCCT}{CG,}{TCGATGATC}{G,}{CA}{AGC,}{GGGAGCG}{TTA,}{GTC}{G,T}{A}{AG,}{TATTGCG}{C,}{CA}{C,}{A,T}{ATGCGCAGG}{AGC,}{G}{TA,}{ATTCAGTCT}{G,}{TG}{C,}{GCCGC}{A,}{AACAA}{T,}{GC}{G,}{GT}{CTT,GGC}{GT}{A,}{C}{G,T}{C,T}{A,T}{CCGGCA}{G,}{GGCTGGGACAT}{TG,}{TGTGTC}{A,G}{A,G}{CCGCAG}{C,}{T}{C,T}{C,T}";


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

ed_string_w = "{GT,}{A,C}{T}{A,G}{ATGCGCTT}{TG,}{TGTTG}{GC,}{GCGGTGGCTT}{AC,G}";
ed_string_q =
    "{AT,}{GAT}{C,G}{C}{G,T}{C}{AGG,TT}{T}{G,T}{TGTTGGCGCGGTGGCTT}{AC,G}";
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

// ed_string_w = "{GC}{AC,G}";
// ed_string_q = "{G,T}{GC}{AC,G}";
ed_string_w = "{GCGGTGGCTT}{AC,G}";
ed_string_q = "{G,T}{GCGGTGGCTT}{AC,G}";
eds_w = parser::parse_ed_string(ed_string_w, params);
eds_q = parser::parse_ed_string(ed_string_q, params);
// run_n_print();
IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
        improved::intersect(eds_w, eds_q, params));
// exit(1);

ed_string_w = "{AT,TC}{ATC,T}";
ed_string_q = "{,G}AT{CT,T}";
// ed_string_w = "{AT,TC}";
// ed_string_q = "{,G}AT";
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

ed_string_w = "{AT,TC}{ATC,}{T}";
ed_string_q = "{TC,G}{CT,T}";
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

ed_string_w = "{GT,}{A,C}{T}";
ed_string_q = "{AT,}{G}{T}";
eds_w = parser::parse_ed_string(ed_string_w, params);
eds_q = parser::parse_ed_string(ed_string_q, params);
IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
        improved::intersect(eds_w, eds_q, params));

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

ed_string_w = "{AT,TC}{TC,T}";
ed_string_q = "TC{,G}{CT,T}";
eds_w = parser::parse_ed_string(ed_string_w, params);
eds_q = parser::parse_ed_string(ed_string_q, params);
IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
        improved::intersect(eds_w, eds_q, params));
// exit(1);

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
ed_string_q = "{AGGA}{G,}{TT}{A,}{GAATATGGCATTCAGTAATCCC}{T,}{T}{CGG,T}{C}{CG,}"
              "{GATGATCGCAGGGAGCGTT}";
eds_w = parser::parse_ed_string(ed_string_w, params);
eds_q = parser::parse_ed_string(ed_string_q, params);
IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
        improved::intersect(eds_w, eds_q, params));

ed_string_w = "{AAC}{CAC,GGT}{TTTTCCATGAC}{A,}{AGGA}{G,}{TTGAATATGGCATTC}{A,C}";
ed_string_q = "{AACCACTTTTCCATGAC}{A,}{AGGA}{G,}{TT}{A,}{"
              "GAATATGGCATTCAGTAATCCCTTC}{GGCCG,}";
eds_w = parser::parse_ed_string(ed_string_w, params);
eds_q = parser::parse_ed_string(ed_string_q, params);
IS_TRUE(naive::intersect(eds_w, eds_q, params) ==
        improved::intersect(eds_w, eds_q, params));
}

void test_lacks_intersect() {
  auto params = init_tests();

  std::string ed_string_w = "{AT,TC}{ATC,T}";
  std::string ed_string_q = "{TC,G}{CG,G}";
  EDS eds_w = parser::parse_ed_string(ed_string_w, params);
  EDS eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));
}

