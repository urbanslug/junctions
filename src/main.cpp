#include <bits/stdc++.h>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <utility>


#include "./cli/cli.hpp"

int main(int argc, char **argv) {
  // CLI
  CommandLineProcessing::ArgvParser cmd;

  // Setup command line options
  cli::initCmdParser(cmd);

  // Parse command line arguments
  n_core::Parameters parameters;
  cli::parseandSave(argc, argv, cmd, parameters);

  app::dispatch(parameters);

  return 0;
}
