#include "./cli/cli.hpp"

#include "./core/core.hpp"



/**
 * junctions entrypoint
 */
int main(int argc, char **argv) {
  // Setup command line options
  core::Parameters parameters;
  
  // parse command line args and run the app
  cli::cli(argc, argv, parameters);

  return 0;
}
