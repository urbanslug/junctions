#include <chrono>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "./args.hxx"

#include "./app.hpp"
#include "../core/core.hpp"
#include "../core/utils.hpp"



namespace cli {

/**
 * @brief                   Print the parsed cmd line options
 * @param[in]  app_config   app_config parsed from command line
 */
void printCmdOptions(core::AppConfig &app_config) {
  auto algo_string =[](core::algorithm algo) -> std::string {
	switch (algo) {
	case core::algorithm::improved:
	  return "improved";
	case core::algorithm::naive:
	  return "naive";
	case core::algorithm::both:
	  return "both";
	}
	return "unknown";
  };

  // TODO: use method to check for the task
  if (app_config.get_task() == core::task::info) {
	std::cerr << "Input files: \n";
	for (auto f: app_config.input_files) {
	  std::cerr << "\t" << f.second << " (format: " << (f.first == 0 ? "msa" : "eds") << ")" << std::endl;
	}
  }
  else {
	std::cerr << "w = " << app_config.w_file_path << " (format: " << (app_config.w_format == 0 ? "msa" : "eds") << ")" << std::endl;
	std::cerr << "q = " << app_config.q_file_path << " (format: " << (app_config.q_format == 0 ? "msa" : "eds") << ")" << std::endl;
  }

  std::cerr << "Task(s):\n";
  switch (app_config.get_task()) {
  case core::task::compute_graph:
	std::cerr << core::indent(1) << "compute intersection graph\n";
	break;
  case core::task::check_intersection:
	std::cerr << core::indent(1) << "check intersection\n";
	std::cerr << core::indent(1) << "algorithm = " << algo_string(app_config.algo) << "\n";
	break;
  case core::task::info:
	std::cerr << core::indent(1) << "print info\n";
	break;
  case core::task::unset:
	break;
  default:
	std::cerr << "unhandled task: report a bug\n";
  }


  if (app_config.output_dot) {
	std::cerr << core::indent(1) << "print dot\n";
  }

  if (app_config.size_of_multiset) {
	std::cerr << core::indent(1) << "compute the size of the multiset" << std::endl;
  }

  if (app_config.compute_witness()) {
	std::cerr << core::indent(1)
			  << "witness choice = "
			  << (app_config.witness_choice == core::witness::longest ? "longest" : "shortest")
			  << "\n";
  }

  if (app_config.compute_match_stats) {
	if (app_config.match_stats_str == 1 || app_config.match_stats_str == 2) {
	  std::cerr << core::indent(1)
				<< "match stats: str = " << app_config.match_stats_str
				<< " letter index = " << app_config.match_stats_letter_idx
				<< "\n";
	}
  }

  if (app_config.get_task() == core::task::compute_graph) {
	std::cerr << core::indent(1) << "constraint = " << app_config.constraint << std::endl;
  }

  if (app_config.compute_dist) {
	std::cerr << core::indent(1) << "compute the distance between two ED-strings" << std::endl;
  }

  if (app_config.compute_similarity) {
	std::cerr << core::indent(1) << "compute the similarity between two ED-strings" << std::endl;
  }

  std::cerr << "verbosity = " <<  static_cast<int>(app_config.verbosity()) << " " << app_config.v << "\n";

  std::cerr << std::endl;
}

void graph_handler(args::Subparser &parser, core::AppConfig &app_config) {
  args::Group arguments("Compute the intersection graph and do one of the following");
  args::Flag dot(parser, "dot", "output the intersection graph in dot format [default: false]", {'g', "dot"});
  args::Flag multiset(parser, "multiset", "compute the size of the multiset", {'m', "multiset"});
  args::ValueFlag<int> witness(parser, "witness", "compute the shortest or longest witness \n shortest = 0  \n longest = 1 \n [default: 0]", {'w', "witness"});
  args::ValueFlag<int> constraint(parser, "constraint", "constrain on which edges are created \n unconstrained = 0  \n constrained = 1 \n [default: 0]", {'c', "constraint"});
  args::Flag distance(parser, "distance", "matching statistics distance between two ED-strings", {'d', "distance"});
  args::Flag similarity(parser, "similarity", "matching statistics similarity between two ED-strings", {'s', "similarity"});

  args::PositionalList<std::string> pathsList(parser, "paths", "paths to ED-strings to compare", args::Options::Required);

  parser.Parse();

  app_config.set_task(core::task::compute_graph);

  if (bool{dot}) {
	app_config.output_dot = true;
  }

  if (bool{multiset}) {
	  app_config.size_of_multiset = true;
  }

  if (bool{witness}) {
	app_config.set_witness(true);
	if (args::get(witness) == 0) {
	  app_config.witness_choice = core::witness::shortest;
	}
	else if (args::get(witness) == 1) {
	  app_config.witness_choice = core::witness::longest;
	}
	else {
	  app_config.witness_choice = core::witness::shortest;
		  //std::cerr << "witness choice must be either 0 or 1" << std::endl;
		  //throw args::Help("witness");
		  //std::exit(1);
	}
  }

  if (bool{constraint}) {
	if (args::get(constraint) == 0) {
	  app_config.constraint = 0;
	} else if (args::get(constraint) == 1){
	  app_config.constraint = 1;
	} else {
	  app_config.constraint = 0;
	}
  }

  if (bool{distance}) {
	app_config.compute_dist = true;
  }

  if (bool{similarity}) {
	app_config.compute_similarity = true;
  }

  std::vector<std::string> input_file_paths;

   for (auto &&path : pathsList) {
	 input_file_paths.push_back(path);
   }

   if (input_file_paths.size() != 2) {
	 std::cerr << "graph command expects exactly 2 input files" << std::endl;
	 throw args::Help("graph");
   }

   // assume 2 input files
   app_config.w_format = utils::extract_extension(input_file_paths[0]);
   app_config.w_file_path = input_file_paths[0];

   app_config.q_format = utils::extract_extension(input_file_paths[1]);
   app_config.q_file_path = input_file_paths[1];
}

void info_handler(args::Subparser &parser, core::AppConfig &app_config) {
  args::Group arguments("info on given input files");
  args::PositionalList<std::string> pathsList(parser, "paths", "input file paths", args::Options::Required);

  parser.Parse();

  app_config.set_task(core::task::info);

  for (auto &&path : pathsList) {
	app_config.input_files.push_back(std::make_pair(utils::extract_extension(path), path));
  }

  std::cout << std::endl;
}

void intersect_handler(args::Subparser &parser, core::AppConfig &app_config) {
  args::Group arguments("do 2 ED strings have a non-empty intersection");
  args::ValueFlag<std::string> algorithm(parser, "algorithm", "algorithm to use: \n improved = 0  \n naive = 1  \n both = 2  \n [default: 0]", {'a', "algorithm"});
  args::PositionalList<std::string> pathsList(parser, "paths", "paths to ED-strings to compare");

  parser.Parse();

   app_config.set_task(core::task::check_intersection);

  if (algorithm) {
	std::string algo_str_arg = args::get(algorithm);

	if (algo_str_arg == "0") {
	  app_config.algo = core::improved;
	}
	else if (algo_str_arg == "1") {
	  app_config.algo = core::naive;
	}
	else if (algo_str_arg == "2") {
	  app_config.algo = core::both;
	}
	else {
	  app_config.algo = core::improved;
	}
  }
  else {
	app_config.algo = core::improved;
  }

  std::vector<std::string> input_file_paths;

   for (auto &&path : pathsList) {
	 input_file_paths.push_back(path);
   }

   if (input_file_paths.size() != 2) {
	 std::cerr << "intersect command expects exactly 2 input files" << std::endl;
	 throw args::Help("intersect");
	 //throw std::length_error("intersect command expects exactly 2 input files");
   }

   // assume 2 input files
   app_config.w_format = utils::extract_extension(input_file_paths[0]);
   app_config.w_file_path = input_file_paths[0];

   app_config.q_format = utils::extract_extension(input_file_paths[1]);
   app_config.q_file_path = input_file_paths[1];
}

int cli(int argc, char **argv, core::AppConfig &app_config) {

  args::ArgumentParser p("junctions", "junctions: a tool for comparing ED-strings");
  args::Group commands(p, "commands");
  args::Command graph(commands,
					  "graph",
					  "compute the intersection graph",
					  [&](args::Subparser &parser) { graph_handler(parser, app_config); });

  args::Command intersect(commands,
						  "intersect",
						  "check for non-empty intersection",
						  [&](args::Subparser &parser) { intersect_handler(parser, app_config); });

  args::Command info(commands,
					 "info",
					 "info on the input files",
					 [&](args::Subparser &parser) { info_handler(parser, app_config); });

  args::Group arguments(p, "arguments", args::Group::Validators::DontCare, args::Options::Global);
  args::Flag version(arguments, "version", "print the current version of junctions", {"version"});
  args::ValueFlag<int> verbosity(arguments, "verbosity", "Level out output", {'v', "verbosity"});
  args::HelpFlag h(arguments, "help", "help", {'h', "help"});

  try {
	p.ParseCLI(argc, argv);
  }
  catch (args::Help) {
	std::cout << p;
	return 1;
  }
  catch (args::Error& e) {
	// only run this if the user is not requesting to print the version
	if (!version) {
	  std::cerr << e.what() << std::endl << p;
	  return 1;
	}
  }

  if (version) {
	std::cout << core::constants::JUNCTIONS_VERSION << std::endl;
	return 0;
  }

  if (args::get(verbosity)) {
	app_config.set_verbosity(args::get(verbosity));
	printCmdOptions(app_config);
  }
  else {
	app_config.set_verbosity(0);
  }

  app::dispatch(app_config);

  return 0;
}
} // namespace cli
