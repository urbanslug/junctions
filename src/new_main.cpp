#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <utility>

#include "./argvparser.hpp"
#include "./core/core.hpp"
#include "./eds/eds.hpp"
#include "./graph/graph.hpp"
#include "./intersect/intersect.hpp"

int main(int argc, char **argv) {

  eds::EDS w, q;
  std::string ed_string_w, ed_string_q;
  int shortest, longest;
  bool res_i, res_n;

  // ed_string_w = "{TC}{G,}{G}";
  // ed_string_q = "{T}{CG,}{TCG}";

  // ed_string_w = "{GT,}{A,C}{T}{A,G}{ATGCGCTT}";
  // ed_string_q = "{G,}{AT}{CC,GA}{T}{C,G}{A,C}{G}{C,G}{TT}";

  // ed_string_w = "{TTC}{A,GC}{CATT}{AC,}{ACTGGCCCGCGCGGTGG}";
  // ed_string_q = "{TTCAC}{AT,C}{C,T}{AC}{T,}{TG}{A,}{GCCCGCGCGGTGGT}";

  ed_string_w = "{T}{C}";
  ed_string_q = "T{,G}{C}";

  ed_string_w = "AAGAA{AG}TCA";
  ed_string_q = "AA{GA}AAGTCA";

   ed_string_w = "{T}{C}";
  ed_string_q = "T{,G}{C}";

  ed_string_w = "CT{G,C}CACC";
  ed_string_q = "C{TG,AT,A}CACC";

  ed_string_w = "A{C,}";
  ed_string_q = "{C,}";




  ed_string_w =
      "{ATTCAGTCT}{G,}{TG}{C,}{GCCGC}{A,}{AACAA}{T,}{GC}{G,}{GT}{CTT,GGC}{"
      "GT}{A,}{C}{G,T}{C,T}{A,T}{CCGGCA}{G,}{GGCTGGGACAT}{TG,}{TGTGTC}{A,G}{A,"
      "G}{CCGCAG}{C,}{T}{C,T}{C,T}";

  ed_string_q = "{ATTCAGTCTGTGGCCGCAACAA}{GCG,}{T}{G,}{GCGTCTTACCGGCA}{G,}"
                "{GGCTGGGACATTGTGTGTC}{AG,GA}{CCGCAG}{CTTT,TCAC}";

  ed_string_w =
    "{ATTCAGTCT}{G,}{TG}{C,}{GCCGC}{A,}{AACAA}{T,}{GC}{G,}{GT}{CTT,GGC}";
  ed_string_q = "{ATTCAGTCTGTGGCCGCAACAA}{GCG,}{T}{G,}{GCGTCTT}";

  ed_string_w = "{GC}{G,}{GT}{CTT,GGC}";
  ed_string_q = "{GCGTCTT}";



  ed_string_w = "{AGGA}{G,}{TTGAATATGGCATTC}{A,C}{GTAATCCCT}{CG,}{TCGATGATC}";
  ed_string_q = "{AGGA}{G,}{TT}{A,}{GAATATGGCATTCAGTAATCCCTTC}";

  ed_string_w = "{GT,}{A,C}{T}";
  ed_string_q = "{AT,}{G}{T}";

  w = eds::Parser::from_string(ed_string_w);
  q = eds::Parser::from_string(ed_string_q);

  w = eds::Parser::from_eds("./data/simulated_d1/x1.d1.s5.l5.w100k.eds");
  q = eds::Parser::from_eds("./data/simulated_d1/x2.d1.s5.l5.w100k.eds");


  //w = eds::Parser::from_eds("./a.eds");
  //q = eds::Parser::from_eds("./b.eds");

  n_core::Parameters params;
  params.verbosity = 0;

  w.print_properties();
  q.print_properties();

  res_i = intersect::improved::has_intersection(w, q, params);
  std::cerr << "res i: " << res_i;
  res_n = intersect::naive::has_intersection(w, q);
  std::cerr << " res n: " << res_n;
  return 0;

  graph::Graph g = graph::compute_intersection_graph(w, q, params);
  longest = graph::longest_witness(g);
  shortest = graph::shortest_witness(g);

  std::cerr << "l: " << longest << "\ns: " << shortest << "\n";

  g.print_dot();
  return 0;
}
