#include "./cli/cli.hpp"

int main(int argc, char **argv) {

  // CLI
  CommandLineProcessing::ArgvParser cmd;

  // Setup command line options
  cli::initCmdParser(cmd);

  // Parse command line arguments
  core::Parameters parameters;
  cli::parseandSave(argc, argv, cmd, parameters);

  app::dispatch(parameters);

  return 0;
}
