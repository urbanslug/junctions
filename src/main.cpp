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
#include "./parseCmdArgs.hpp"
#include "./core.hpp"

int main(int argc, char **argv) {
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

  if (parameters.verbosity > 0) {
    std::cerr << "INFO, [junctions::main]"
              << " N: " << w.size << " n: " << w.m
              << " M: " << q.size << " m: " << q.m
              << std::endl;
  }

  // utils::print_edt_data(w.data);

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

  return 0;
}
