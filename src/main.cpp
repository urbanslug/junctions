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
      std::cerr << "INFO [junctions::main] Time spent by naive algorithm: "
                << timeRefRead.count() << " sec" << std::endl;
    }

  } else if (parameters.algo == core::algorithm::improved) {
    // Improved
    // --------

    result = improved::intersect(w, q, parameters);

    timeRefRead = Time::now() - t0;

    if (parameters.verbosity > 0) {
      std::cerr << "INFO [junctions::main] Time spent by improved algorithm:"
                << timeRefRead.count() << " sec" << std::endl;
    }
  } else if (parameters.algo == core::algorithm::both) {
    // Both (for testing)
    // ------------------

    bool result_improved = improved::intersect(w, q, parameters);
    timeRefRead = Time::now() - t0;

    if (parameters.verbosity > 0) {
      std::cerr << "INFO [junctions::main] Time spent by improved algorithm: "
                << timeRefRead.count() << " sec" << std::endl;
    }

    t0 = Time::now();
    bool result_naive = naive::intersect(w, q, parameters);
    timeRefRead = Time::now() - t0;

    if (parameters.verbosity > 0) {
      std::cerr << "INFO [junctions::main] Time spent by naive algorithm: "
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

  std::cout << "INFO [junctions::main] "
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

  if (parameters.verbosity > 0) {
    std::cerr << "INFO [junctions::main::do_graph] computing intersection graph " << std::endl;
  }

  graph::Graph g = graph::compute_intersection_graph(w, q, parameters);

  timeRefRead = Time::now() - t0;

  if (parameters.verbosity > 0) {
    std::cerr
        << "INFO [junctions::main::do_graph] Time spent computing intersection graph: "
        << timeRefRead.count() << " sec" << std::endl;
  }

  // dot
  if (parameters.output_dot) {
    if (parameters.verbosity > 0) {
      std::cerr
          << "INFO [junctions::main::do_graph] generating graph in dot format"
          << std::endl;
    }
      g.print_dot();
  }

  if (parameters.size_of_multiset) {
      if (parameters.verbosity > 0) {
      std::cerr
          << "INFO [junctions::main::do_graph] computing the size of the multiset"
          << std::endl;
      }
    std::cout << "Size of the mutlitset: " << g.multiset_size() << std::endl;
  }

  if (parameters.compute_witness) {
    int witness_len;
    // TODO: confirm that an intersection exists
    switch (parameters.witness_choice) {
    case core::witness::longest:
      if (parameters.verbosity > 0) {
        std::cerr << "INFO [junctions::main] computing the longest witness " <<  std::endl;
      }

      t0 = Time::now();
      witness_len = graph::longest_witness(g);
      timeRefRead = Time::now() - t0;

      std::cout << "longest witness is: " << witness_len << " chars long." << std::endl;

      if (parameters.verbosity > 0) {
        std::cerr << "INFO [junctions::main] Time spent computing the longest "
                     "witness: "
                  << timeRefRead.count() << " sec" << std::endl;
      }

      break;

    case core::witness::shortest:
      if (parameters.verbosity > 0) {
        std::cerr << "INFO [junctions::main] computing the shortest witness " << std::endl;
      }

      t0 = Time::now();
      witness_len =  graph::shortest_witness(g);
      timeRefRead = Time::now() - t0;

      std::cout << "shortests witness is: " << witness_len << " chars long." <<  std::endl;

      if (parameters.verbosity > 0) {
        std::cerr << "INFO [junctions::main] Time spent computing the shortest "
                     "witness: "
                  << timeRefRead.count() << " sec" << std::endl;
      }
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

EDS read_file(core::Parameters &parameters, std::pair<core::file_format, std::string> file) {
  std::string eds_string, file_path;
  EDS ed_string;
  std::vector<std::string> msa_data;

  core::file_format format = file.first;
  file_path = file.second;

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
    std::cerr << "[junctions::read_file] Error: Could not figure out format for " << file.second << std::endl;
    exit(1);
  }

  return ed_string;
}

void print_info(EDS f, int indent_level = 0) {
  std::cout << utils::indent(indent_level) << f.size << "\t" << f.m << std::endl;
}

int main(int argc, char **argv) {

  // CLI
  CommandLineProcessing::ArgvParser cmd;

  // Setup command line options
  cli::initCmdParser(cmd);

  // Parse command line arguments
  core::Parameters parameters;

  cli::parseandSave(argc, argv, cmd, parameters);

  if (parameters.verbosity > 1) { std::cerr << "DEBUG [junctions::main]" << std::endl; }

  // Read files
  EDS q, w;

  // TODO: combine with loop
  // print (debug) info
  auto foo = [&]() {
    if (parameters.verbosity > 0) {
      std::cerr << utils::indent(1) << "N_1: " << w.size << " m_1: " << w.m
                << " N_2: " << q.size << " m_2: " << q.m << std::endl;
    }
  };

  auto loop = [&](){

    EDS i;
    std::cout << "File\tN\tm" << std::endl;
    for (auto f: parameters.input_files) {
      i = read_file(parameters, f);
      std::cerr << f.second;
      print_info(i, 1);
    }
  };

  // utils::print_edt_data(w.data);

  switch (parameters.task) {
  case core::arg::compute_graph:
    q = read_files(parameters, core::ed_string::q);
    w = read_files(parameters, core::ed_string::w);

    // foo();

    do_graph(w, q, parameters);
    break;
  case core::arg::check_intersection:
    q = read_files(parameters, core::ed_string::q);
    w = read_files(parameters, core::ed_string::w);

    foo();
    do_intersect(w, q, parameters);
    break;
  case core::arg::info:
    loop();
    break;
  default:
    std::cerr << "Unhandled task: report a bug" << std::endl;
  }


  return 0;
}
