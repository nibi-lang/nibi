#include <iostream>
#include <vector>
#include <string>

#include <runtime/cell.hpp>
#include <runtime/memory.hpp>
#include "runtime/environment.hpp"
#include "runtime/runtime.hpp"
#include "common/error.hpp"
#include "common/source.hpp"
#include "common/input.hpp"
#include "common/list.hpp"

namespace {
  // The number of instructions that can be executed before
  // the instruction memory is swept to free up cells
  constexpr std::size_t INSTRUCTIONS_ALLOWED_BEFORE_SWEEP = 50;

  // Instruction manager used by all list builders
  cell_memory_manager_t* instruction_memory{nullptr};

  // The object that will be used as the top level env
  env_c* program_global_env{nullptr};

  // Source input manager used to ensure that files are only read once
  // and to generate locators for cells
  source_manager_c* source_manager{nullptr};
}

void teardown() {
  global_runtime_destroy();
  global_cells_destroy();
  delete source_manager;
  delete program_global_env;
  delete instruction_memory;
}

void setup() {
  instruction_memory = new cell_memory_manager_t(INSTRUCTIONS_ALLOWED_BEFORE_SWEEP);
  program_global_env = new env_c(nullptr);
  source_manager = new source_manager_c();

  // Initialize the global cells (true, false, nil, etc)
  if (!global_cells_initialize()) {
    std::cerr << "Failed to initialize global cells" << std::endl;
    teardown();
    exit(1);
  }

  // Initialize the global runtime object
  if (!global_runtime_init(*program_global_env, *source_manager, *instruction_memory)) {
    std::cerr << "Failed to initialize global runtime" << std::endl;
    teardown();
    exit(1);
  }
}

void run_from_file(const std::string& file_name) {

  // List builder that will build lists from parsed tokens
  // and pass lists to a runtime
  list_builder_c list_builder(*instruction_memory, *global_runtime);

  // File reader that reads file and kicks off parser/ scanner 
  // that will send tokens to the list builder
  file_reader_c file_reader(list_builder, *source_manager);

  // Read the file and start the process
  file_reader.read_file(file_name);
}

int main(int argc, char** argv) {

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
