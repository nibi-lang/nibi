#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "common/error.hpp"
#include "common/input.hpp"
#include "common/list.hpp"
#include "interpreter/interpreter.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/environment.hpp"
#include "libnibi/source.hpp"

namespace {

// The object that will be used as the top level env
env_c *program_global_env{nullptr};

// Source input manager used to ensure that files are only read once
// and to generate locators for cells
source_manager_c *source_manager{nullptr};
} // namespace

void teardown() {
  global_interpreter_destroy();
  delete source_manager;
  delete program_global_env;
}

void setup() {
  program_global_env = new env_c(nullptr);
  source_manager = new source_manager_c();

  // Initialize the global interpreter object
  if (!global_interpreter_init(*program_global_env, *source_manager)) {
    std::cerr << "Failed to initialize global interpreter" << std::endl;
    teardown();
    exit(1);
  }
}

void run_from_file(const std::string &file_name) {

  // List builder that will build lists from parsed tokens
  // and pass lists to a interpreter
  list_builder_c list_builder(*global_interpreter);

  // File reader that reads file and kicks off parser/ scanner
  // that will send tokens to the list builder
  file_reader_c file_reader(list_builder, *source_manager);

  // Read the file and start the process
  file_reader.read_file(file_name);
}

int main(int argc, char **argv) {

  std::vector<std::string> args(argv, argv + argc);

  if (args.size() == 1) {
    std::cout << "No input file specified" << std::endl;
    return 1;
  }

  auto entry_file = args[1];

  setup();

  run_from_file(entry_file);

  teardown();

  return 0;
}
