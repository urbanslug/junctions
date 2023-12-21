#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <ios>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>

#include "../eds/eds.hpp"
#include "../core/core.hpp"
#include "../core/utils.hpp"
#include "../graph/graph.hpp"
#include "../intersect/intersect.hpp"


namespace app {

void handle_graph(core::AppConfig const &params);
void handle_intersection(core::AppConfig const &params);
void handle_info(core::AppConfig const &params);

void dispatch(core::AppConfig const& params) {
  switch (params.get_task()) {
  case core::task::compute_graph:
	handle_graph(params);
	break;
  case core::task::check_intersection:
	handle_intersection(params);
	break;
  case core::task::info:
	handle_info(params);
	break;
  case core::task::unset:
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
void handle_info(core::AppConfig const &params) {
  eds::EDS e;
  // TODO: file format?
  // TODO: which is the right place to check the file format?
  // TODO: not use magic number
  std::size_t w{20}; // width
  std::size_t tw{20}; // title width

  std::cout << std::left << std::setw(tw) << "Size (N)"
			<< std::left << std::setw(tw) << "Length (n)"
			<< std::left << std::setw(tw) << "String Count (m)"
			<< std::left << std::setw(tw) << "Epsilon Count"
			<< std::left << std::setw(tw) << "File"
			<< "\n";
  for (auto fp: params.input_files) {
	core::file_format format = utils::extract_extension(fp.second);
#ifdef AVX2_SUPPORTED
	e = format == core::file_format::msa ?
	  eds::Parser::from_msa(fp.second):
	  eds::Parser::from_eds(fp.second);
#else
	if (format == core::file_format::msa) {
	  std::cerr << "[junctions::app::handle_info] ERROR: junctions was not compiled with MSA support\n";
	  exit(1);
	}
	e = eds::Parser::from_eds(fp.second);
#endif

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
void handle_intersection(core::AppConfig const &params) {
  eds::EDS w, q;

#ifdef AVX2_SUPPORTED
  w = params.w_format == core::file_format::msa ?
	eds::Parser::from_msa(params.get_w_fp().second) :
	eds::Parser::from_eds(params.get_w_fp().second);

  q = params.q_format == core::file_format::msa ?
	eds::Parser::from_msa(params.get_q_fp().second) :
	eds::Parser::from_eds(params.get_q_fp().second);
#else
  	if (params.w_format == core::file_format::msa || params.q_format == core::file_format::msa) {
	  std::cerr << "[junctions::app::handle_intersection] ERROR: junctions was not compiled with MSA support\n";
	  exit(1);
	}

	w =  eds::Parser::from_eds(params.get_w_fp().second);
	q = eds::Parser::from_eds(params.get_q_fp().second);
#endif
  
  // w = eds::Parser::from_eds(params.get_w_fp().second);
  // q = eds::Parser::from_eds(params.get_q_fp().second);

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

  auto t0 = core::Time::now();

  switch (params.get_algo()) {
  case core::algorithm::improved: {
	res_i = intersect::improved::has_intersection(w, q);
	timeRefRead = core::Time::now() - t0;
	report_res(res_i);
	report_time("improved");
  }
	break;
  case core::algorithm::naive: {
	res_n = intersect::naive::has_intersection(w, q);
	timeRefRead = core::Time::now() - t0;
	report_res(res_n);
	report_time("naive");
  }
	break;
  case core::algorithm::both: {
	// Both (for tests & benchmarks)
	// -----------------------------
	res_i = intersect::improved::has_intersection(w, q);
	timeRefRead = core::Time::now() - t0;
	report_res(res_i);
	report_time("improved");

	t0 = core::Time::now();
	res_n = intersect::naive::has_intersection(w, q);
	timeRefRead = core::Time::now() - t0;

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
void handle_graph(core::AppConfig const &app_config) {
  eds::EDS w, q;
  // w = eds::Parser::from_eds(params.get_w_fp().second);
  // q = eds::Parser::from_eds(params.get_q_fp().second);
  /*
#ifndef AVX2_SUPPORTED
	if (params.w_format == core::file_format::msa || params.q_format == core::file_format::msa) {
	  std::cerr << "[junctions::app::handle_graph] ERROR: junctions was not compiled with MSA support\n";
	  exit(1);
	}
#endif
*/
  
#ifdef AVX2_SUPPORTED
  w = app_config.w_format == core::file_format::msa ?
	eds::Parser::from_msa(app_config.get_w_fp().second) :
	eds::Parser::from_eds(app_config.get_w_fp().second);

  q = app_config.q_format == core::file_format::msa ?
	eds::Parser::from_msa(app_config.get_q_fp().second) :
	eds::Parser::from_eds(app_config.get_q_fp().second);
#else
	if (app_config.w_format == core::file_format::msa || app_config.q_format == core::file_format::msa) {
	  std::cerr << "[junctions::app::handle_graph] ERROR: junctions was not compiled with MSA support\n";
	  exit(1);
	}

	w = eds::Parser::from_eds(app_config.get_w_fp().second);
	q = eds::Parser::from_eds(app_config.get_q_fp().second);
#endif
  
  std::chrono::duration<double> timeRefRead;
  auto t0 = core::Time::now();

  if (app_config.verbosity() > 0) {
	std::cerr << "INFO computing intersection graph\n";
  }

  graph::Graph g = graph::compute_intersection_graph(w, q, app_config);
  timeRefRead = core::Time::now() - t0;

  if (app_config.verbosity() > 0) {
	std::cerr << "INFO Time spent computing intersection graph: "
			  << timeRefRead.count() << " sec\n";
  }

  // generate the graph in dot format
  if (app_config.gen_dot()) {
	if (app_config.verbosity() > 0) {
	  std::cerr << "INFO generating graph in dot format\n";
	}
	  g.print_dot();
  }

  // compute the size of the multiset
  if (app_config.multiset()) {
	if (app_config.verbosity() > 0) {
	  std::cerr << "INFO computing the size of the multiset\n";
	  }
	std::cout << "Size of the mutlitset: " << graph::multiset(g) << "\n";
  }

  
  // compute the length of the longest witness
  if (app_config.compute_witness()) {
	int witness_len;
	switch (app_config.get_witness_choice()) {
	case core::witness::longest:
	  if (app_config.verbosity() > 0) {
		std::cerr << "INFO computing the longest witness\n";
	  }

	  t0 = core::Time::now();
	  witness_len = graph::longest_witness(g);
	  timeRefRead = core::Time::now() - t0;

	  std::cout << "longest witness is: " << witness_len << " chars long.\n";

	  if (app_config.verbosity() > 0) {
		std::cerr << "INFO Time spent computing the longest witness: "
				  << timeRefRead.count() << " sec\n";
	  }

	  break;

	case core::witness::shortest:
	  if (app_config.verbosity() > 0) {
		std::cerr << "INFO computing the shortest witness\n";
	  }

	  t0 = core::Time::now();
	  witness_len =  graph::shortest_witness(g);
	  timeRefRead = core::Time::now() - t0;

	  std::cout << "shortests witness is: " << witness_len << " chars long.\n";

	  if (app_config.verbosity() > 0) {
		std::cerr << "INFO Time spent computing the shortest witness: "
				  << timeRefRead.count() << " sec\n";
	  }
	  break;

	default:
	  break;
	}
  }

  if (app_config.compute_dist) {
	//std::size_t res{std::numeric_limits<std::size_t>::max()};
	std::double_t distance{};
	distance = graph::distance(g, w, q);
	std::cout << "Distance measure is: " << distance << "\n";
  }

  
  if (app_config.compute_similarity) {
	//std::size_t res{std::numeric_limits<std::size_t>::max()};
	std::double_t distance{};
	distance = graph::similarity(g, w, q);
	std::cout << "Similarity measure is: " << distance << "\n";
  }
  
  // TODO replace with method call
  if (app_config.compute_match_stats) {

	std::size_t res{std::numeric_limits<std::size_t>::max()};
	std::double_t avg{};

	if (app_config.verbosity() > 0) {
	  std::cerr << "INFO computing matching statistics\n";
	}


	if (app_config.compute_match_stats_avg) {
	  avg = graph::match_stats_avg(g, w, q);
	}
	else {
	switch (app_config.match_stats_str) {
	case 1:
	  res = graph::match_stats(g,
							   w.get_letter_boundaries(app_config.match_stats_letter_idx).left(),
							   q.get_size() + q.get_eps_count(),
							   core::ed_string::w);
	  break;
	case 2:
	  res = graph::match_stats(g,
							   q.get_letter_boundaries(app_config.match_stats_letter_idx).left(),
							   w.get_size() + w.get_eps_count(),
							   core::ed_string::q);
	  break;
	default:
	  throw std::invalid_argument("invalid match stats string");
	}
	}

	if (app_config.compute_match_stats_avg) {
	  //std::cout << "MS average is: " << std::fixed << std::setprecision(2) << avg << "\n";
	  std::cout << "MS average is: " << avg << "\n";
	}
	else {
	  std::cout << "MS[" << app_config.match_stats_letter_idx << "]: " << res << "\n";
	}
  }
}

} // namespace app
