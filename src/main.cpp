#include "./cli/cli.hpp"

#include "./core/core.hpp"



/**
 * junctions entrypoint
 */
int main(int argc, char **argv) {
  // Setup command line options
  core::AppConfig app_config;
  
  // parse command line args and run the app
  cli::cli(argc, argv, app_config);

  return 0;
}
