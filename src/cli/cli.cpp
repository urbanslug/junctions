#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>

#include "../core/core.hpp"
#include "../graph/graph.hpp"
#include "../intersect/intersect.hpp"
#include "../eds/eds.hpp"

namespace app {

void handle_graph(n_core::Parameters const &params);
void handle_intersection(n_core::Parameters const &params);
void handle_info(n_core::Parameters const &params);

void dispatch(n_core::Parameters const& params) {
  switch (params.get_task()) {
  case n_core::task::compute_graph:
    handle_graph(params);
    break;
  case n_core::task::check_intersection:
    handle_intersection(params);
    break;
  case n_core::task::info:
    handle_info(params);
    break;
  default:
    std::string e = "Unhandled task: report a bug\n";
    throw std::invalid_argument(e);
  }
}

// ----
// info
// ----

/**
 *
 */
void handle_info(n_core::Parameters const &params) {
  eds::EDS e;
  // TODO: file format?
  // TODO: which is the right place to check the file format?
  // TODO: not use magic number
  std::size_t w{20}; // width
  std::size_t tw{20}; // title width

  std::cout << std::left << std::setw(tw)  << "size(N)"
            << std::left << std::setw(tw) << "length(n)"
            << std::left << std::setw(tw) << "str count(m)"
            << std::left << std::setw(tw) << "number of epsilons"
            << std::left << std::setw(tw)  << "file"
            << "\n";
  for (auto fp: params.input_files) {
    e = eds::Parser::from_eds(fp.second);
    std::cout << std::left << std::setw(w) << e.get_size()
              << std::left << std::setw(w) << e.get_length()
              << std::left << std::setw(w) << e.get_str_count()
              << std::left << std::setw(w) << e.get_eps_count()
              << std::left << std::setw(w) << fp.second
              << "\n";
  }
}

// ------------
// intersection
// ------------

/**
 * Compute intersection
 */
void handle_intersection(n_core::Parameters const &params) {
  eds::EDS w, q;
  w = eds::Parser::from_eds(params.get_w_fp().second);
  q = eds::Parser::from_eds(params.get_q_fp().second);

  bool res_i{false}, res_n{false};
  std::chrono::duration<double> timeRefRead;

  auto report_res = [](bool res) {
    std::cout << "INFO "
              << (res ? "intersection exists" : "no intersection")
              << "\n";
  };

  auto err = [&]() {
    std::cerr << "INFO incompatible results "
              << "(naive " << res_n << " improved " << res_i
              << "). Please report as a bug.\n";
    exit(1);
  };

  auto report_time = [&](std::string const& n) {
    if (params.verbosity() > 0) {
      std::cerr << "INFO Time spent by " + n + " algorithm: "
                << timeRefRead.count() << " sec\n";
    }
  };

  auto t0 = n_core::Time::now();

  switch (params.get_algo()) {
  case n_core::algorithm::improved: {
    res_i = intersect::improved::has_intersection(w, q);
    timeRefRead = n_core::Time::now() - t0;
    report_res(res_i);
    report_time("improved");
  }
    break;
  case n_core::algorithm::naive: {
    res_n = intersect::naive::has_intersection(w, q);
    timeRefRead = n_core::Time::now() - t0;
    report_res(res_n);
    report_time("naive");
  }
    break;
  case n_core::algorithm::both: {
    // Both (for tests & benchmarks)
    // -----------------------------
    res_i = intersect::improved::has_intersection(w, q);
    timeRefRead = n_core::Time::now() - t0;
    report_res(res_i);
    report_time("improved");

    t0 = n_core::Time::now();
    res_n = intersect::naive::has_intersection(w, q);
    timeRefRead = n_core::Time::now() - t0;

    if (res_i != res_n) { err(); }

    report_time("naive");
  }
    break;
  default:
    std::string e =
      "could not determine algorithm to use: " +
      std::string(__FILE__) + ":" + std::to_string(__LINE__) +
      " report a bug\n";
    throw std::invalid_argument(e);
    ;
  }
}

// -----
// graph
// -----
/**
 * Compute Intersection Graph and...
 *
 */
void handle_graph(n_core::Parameters const &params) {
  eds::EDS w, q;
  w = eds::Parser::from_eds(params.get_w_fp().second);
  q = eds::Parser::from_eds(params.get_q_fp().second);

  std::chrono::duration<double> timeRefRead;
  auto t0 = n_core::Time::now();

  if (params.verbosity() > 0) {
    std::cerr << "INFO computing intersection graph\n";
  }

  graph::Graph g = graph::compute_intersection_graph(w, q, params);
  timeRefRead = n_core::Time::now() - t0;

  if (params.verbosity() > 0) {
    std::cerr << "INFO Time spent computing intersection graph: "
              << timeRefRead.count() << " sec\n";
  }

  // dot
  if (params.gen_dot()) {
    if (params.verbosity() > 0) {
      std::cerr << "INFO generating graph in dot format\n";
    }
      g.print_dot();
  }

  if (params.multiset()) {
    if (params.verbosity() > 0) {
      std::cerr << "INFO computing the size of the multiset\n";
      }
    std::cout << "Size of the mutlitset: " << g.multiset_size() << "\n";
  }

  if (params.compute_witness()) {
    int witness_len;
    switch (params.get_witness_choice()) {
    case n_core::witness::longest:
      if (params.verbosity() > 0) {
        std::cerr << "INFO computing the longest witness\n";
      }

      t0 = n_core::Time::now();
      witness_len = graph::longest_witness(g);
      timeRefRead = n_core::Time::now() - t0;

      std::cout << "longest witness is: " << witness_len << " chars long.\n";

      if (params.verbosity() > 0) {
        std::cerr << "INFO Time spent computing the longest witness: "
                  << timeRefRead.count() << " sec\n";
      }

      break;

    case n_core::witness::shortest:
      if (params.verbosity() > 0) {
        std::cerr << "INFO computing the shortest witness\n";
      }

      t0 = n_core::Time::now();
      witness_len =  graph::shortest_witness(g);
      timeRefRead = n_core::Time::now() - t0;

      std::cout << "shortests witness is: " << witness_len << " chars long.\n";

      if (params.verbosity() > 0) {
        std::cerr << "INFO Time spent computing the shortest witness: "
                  << timeRefRead.count() << " sec\n";
      }
      break;

    default:
      break;
    }
  }
}

} // namespace app
