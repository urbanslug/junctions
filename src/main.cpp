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
#include "./compute_intersect.cpp"
#include "./graph.cpp"
#include "./parseCmdArgs.hpp"
#include "./core.hpp"
#include "utils.hpp"

/*
 * Compute intersection
 *
 *
 * TODO: rename this function
 */
void do_intersect(EDS w, EDS q, core::Parameters const &parameters) {
  bool result;

  auto t0 = Time::now();
  std::chrono::duration<double> timeRefRead;

  if (parameters.algo == core::algorithm::naive) {
    // Naive
    // -----
    result = naive::intersect(w, q, parameters);

    timeRefRead = Time::now() - t0;

    if (parameters.verbosity > 0) {
      std::cerr << "INFO, [junctions::main] Time spent by naive algorithm: "
                << timeRefRead.count() << " sec" << std::endl;
    }

  } else if (parameters.algo == core::algorithm::improved) {
    // Improved
    // --------

    result = improved::intersect(w, q, parameters);

    timeRefRead = Time::now() - t0;

    if (parameters.verbosity > 0) {
      std::cerr << "INFO, [junctions::main] Time spent by improved algorithm:"
                << timeRefRead.count() << " sec" << std::endl;
    }
  } else if (parameters.algo == core::algorithm::both) {
    // Both (for testing)
    // ------------------

    bool result_improved = improved::intersect(w, q, parameters);
    timeRefRead = Time::now() - t0;

    if (parameters.verbosity > 0) {
      std::cerr << "INFO, [junctions::main] Time spent by improved algorithm: "
                << timeRefRead.count() << " sec" << std::endl;
    }

    t0 = Time::now();
    bool result_naive = naive::intersect(w, q, parameters);
    timeRefRead = Time::now() - t0;

    if (parameters.verbosity > 0) {
      std::cerr << "INFO, [junctions::main] Time spent by naive algorithm: "
                << timeRefRead.count() << " sec" << std::endl;
    }

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
}

/*
 * Compute Intersection Graph
 *
 *
 * TODO: rename this function
 */
void do_graph(EDS w, EDS q, core::Parameters const &parameters) {
  auto t0 = Time::now();
  std::chrono::duration<double> timeRefRead;

  graph::Graph g = graph::compute_intersection_graph(w, q, parameters);

  timeRefRead = Time::now() - t0;

  if (parameters.verbosity > 0) {
    std::cerr
        << "INFO, [junctions::main] Time spent computing intersection graph: "
        << timeRefRead.count() << " sec" << std::endl;
  }

  if (parameters.output_dot) {
    if (parameters.verbosity > 1) { std::cerr << "Generating graph in dot format" << std::endl; }
      g.print_dot();
  }

  if (parameters.size_of_multiset) {
    std::cout << "Size of the mutlitset: " << g.multiset_size() << std::endl;
  }

  if (parameters.compute_witness) {
    // TODO: confirm that an intersection exists
    switch (parameters.witness_choice) {
    case core::witness::longest:
      graph::longest_witness(g);
      break;
    case core::witness::shortest:
      graph::shortest_witness(g);
      break;
    default:
      break;
    }
  }

  if (parameters.compute_match_stats) { graph::match_stats(g, w, q, parameters); }
}

/*
 * Read input files
 *
 *
 * @param[in] parameters
 * @param[in] which_ed_string
 * return
 */
EDS read_files(core::Parameters& parameters, core::ed_string which_ed_string) {

  EDS ed_string;
  std::string eds_string, file_path;
  std::vector<std::string> msa_data;
  core::file_format format;


  switch (which_ed_string) {
  case core::ed_string::w:
    file_path = parameters.w_file_path;
    format = parameters.w_format;
    break;
  case core::ed_string::q:
    file_path = parameters.q_file_path;
    format = parameters.q_format;
    break;
  default:
    exit(1);
  }

  switch (format) {
  case core::file_format::eds:
    eds_string = parser::read_eds(file_path);
    ed_string = parser::parse_ed_string(eds_string, parameters);
    break;
  case core::file_format::msa:
    msa_data = parser::read_msa(file_path);
    eds_string = parser::msa_to_eds(msa_data);
    ed_string = parser::parse_ed_string(eds_string, parameters);
    break;
  default:
    exit(1);
  }

  return ed_string;
}

int main(int argc, char **argv) {

  // CLI
  CommandLineProcessing::ArgvParser cmd;

  // Setup command line options
  cli::initCmdParser(cmd);

  // Parse command line arguments
  core::Parameters parameters;

  cli::parseandSave(argc, argv, cmd, parameters);

  if (parameters.verbosity > 0) {
    std::cerr << "[junctions::main]" << std::endl;
  }

  // Read files
  EDS q = read_files(parameters, core::ed_string::q);
  EDS w = read_files(parameters, core::ed_string::w);

  // print (debug) info
  if (parameters.verbosity > 0) {
    std::cerr << utils::indent(1)
              << "N: " << w.size << " n: " << w.m
              << " M: " << q.size << " m: " << q.m
              << std::endl;
  }

  // utils::print_edt_data(w.data);

  switch (parameters.task) {
  case core::arg::compute_graph:
    do_graph(w, q, parameters);
    break;
  case core::arg::check_intersection:
    do_intersect(w, q, parameters);
    break;
  default:
    std::cerr << "Unhandled task: report a bug" << std::endl;
  }


  return 0;
}
