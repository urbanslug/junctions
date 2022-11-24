#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <utility>


# include "./compute_intersect.cpp"
#include "./argvparser.hpp"
#include "./parseCmdArgs.hpp"
#include "core.hpp"


void test_lacks_intersect();
void test_contains_intersect();
void test_parse_ed_string();
void test_handle_epsilon();
void test_msa_to_eds();


int main(int argc, char **argv) {

  test_handle_epsilon();
  exit(1);

  // ---
  // CLI
  // ---
  CommandLineProcessing::ArgvParser cmd;

  // Setup command line options
  cli::initCmdParser(cmd);

  // Parse command line arguments
  core::Parameters parameters; 

  cli::parseandSave(argc, argv, cmd, parameters);

  // ----------
  // Read files
  // ----------

  std::string eds_string;
  std::vector<std::string> msa_data;
  EDS w, q;
  // TODO: offload to lambda
  if (parameters.q_format == core::file_format::eds) {
    eds_string = parser::read_eds(parameters.q_file_path);
    q = parser::parse_ed_string(eds_string, parameters);
  } else {
    msa_data = parser::read_msa(parameters.q_file_path);
    eds_string = parser::msa_to_eds(msa_data);
    q = parser::parse_ed_string(eds_string, parameters);
  }

  if (parameters.w_format == core::file_format::eds) {
    eds_string = parser::read_eds(parameters.w_file_path);
    w = parser::parse_ed_string(eds_string, parameters);
  } else {
    msa_data = parser::read_msa(parameters.w_file_path);
    eds_string = parser::msa_to_eds(msa_data);
    w = parser::parse_ed_string(eds_string, parameters);
  }

  // --------------------
  // Compute intersection
  // --------------------

  bool result;

  auto t0 = Time::now();
  std::chrono::duration<double> timeRefRead;

  if (parameters.algo == core::algorithm::naive) {

    result = naive::intersect(w, q, parameters);

    timeRefRead = Time::now() - t0;

    if (parameters.verbosity > 0) {
      std::cerr << "INFO, [junctions::main] Time spent by naive algorithm: "
                << timeRefRead.count() << " sec" << std::endl;
    }

  } else if (parameters.algo == core::algorithm::improved) {
    result = improved::intersect(w, q, parameters);

    timeRefRead = Time::now() - t0;

    if (parameters.verbosity > 0) {
      std::cerr << "INFO, [junctions::main] Time spent by improved algorithm:"
                << timeRefRead.count() << " sec" << std::endl;
    }
  } else if (parameters.algo == core::algorithm::both) {

    bool result_improved = improved::intersect(w, q, parameters);
    timeRefRead = Time::now() - t0;

    if (parameters.verbosity > 0) {
      std::cerr << "INFO, [junctions::main] Time spent by improved algorithm: "
                << timeRefRead.count() << " sec" << std::endl;
    }

    bool result_naive = naive::intersect(w, q, parameters);

    timeRefRead = Time::now() - t0;
    if (parameters.verbosity > 0) {
      std::cerr << "INFO, [junctions::main] Time spent by naive algorithm: "
                << timeRefRead.count() << " sec" << std::endl;
    }

    t0 = Time::now();


      if (result_naive != result_improved) {
        std::cerr << "[junctions::main] incompatible results "
                  << "(naive " << result_naive << " improved "
                  << result_improved << "). Please report as a bug."
                  << std::endl;
        exit(1);
      }

      result = result_improved;
    } else {
      std::cerr << "[junctions::main] could not determine algorithm to use"
                << std::endl;
      exit(1);
    }

  std::cout << "INFO, [junctions::main] "
            << (result ? "intersection exists" : "no intersection")
            << std::endl;

  return 0;
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

  ed_string_w = "{GCG}{TTA,}{GTC}{G,T}";
  ed_string_q = "{GCGTT}{AG,T}{T}{AC,CG}";
  eds_w = parser::parse_ed_string(ed_string_w, params);
  eds_q = parser::parse_ed_string(ed_string_q, params);
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));
  exit(1);

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
  std::string ed_string_w = "{AT,TC}{ATC,T}";
  std::string ed_string_q = "{TC,G}{CG,G}";

  auto params = init_tests();

  EDS eds_w = parser::parse_ed_string(ed_string_w, params);
  EDS eds_q = parser::parse_ed_string(ed_string_q, params);

  // IS_TRUE(!improved::intersect(eds_w, eds_q));
  IS_TRUE(naive::intersect(eds_w, eds_q, params) == improved::intersect(eds_w, eds_q, params));
}

