#include <iostream>
#include <string>
#include <vector>

#include "common/error.hpp"
#include "common/input.hpp"
#include "common/list.hpp"
#include "common/source.hpp"
#include "runtime/environment.hpp"
#include "runtime/runtime.hpp"
#include <runtime/cell.hpp>
#include <runtime/memory.hpp>
#include "profile/config.hpp"

namespace {

// Instruction manager used by all list builders
cell_memory_manager_t *cell_memory{nullptr};

// The object that will be used as the top level env
env_c *program_global_env{nullptr};

// Source input manager used to ensure that files are only read once
// and to generate locators for cells
source_manager_c *source_manager{nullptr};
} // namespace


#if PROFILE_ALLOCATOR
void display_allocator_stats() {
  auto total_allocs = cell_memory->num_std_allocations +
                      cell_memory->num_no_sweep_allocations;

  std::cout << "[ MEMORY PROFILE ]" << std::endl;
  std::cout << cell_memory->num_std_allocations << " standard allocations" << std::endl;
  std::cout << cell_memory->num_no_sweep_allocations << " no-sweep allocations" << std::endl;
  std::cout << total_allocs  << " total allocations" << std::endl;
  std::cout << cell_memory->num_ownderships_taken << " ownerships assumed" << std::endl;
  std::cout << cell_memory->num_sweeps << " total sweeps" << std::endl;
  std::cout << cell_memory->num_frees << " frees from sweeps" << std::endl;

  // Flush the memory of all cells (mimic destructor)
  cell_memory->flush();

  std::cout << cell_memory->num_items_flushed << " items flushed" << std::endl;

  // Calculate all freed items to ensure that there were no leaks
  auto total_frees = cell_memory->num_frees + cell_memory->num_items_flushed;
  std::cout << "Lost nodes: " << total_allocs - total_frees << std::endl;
  std::cout << "[ END MEMORY PROFILE ]\n" << std::endl;
}
#endif

void teardown() {
  global_runtime_destroy();
  global_cells_destroy();
  delete source_manager;
  delete program_global_env;

#if PROFILE_ALLOCATOR
  display_allocator_stats();
#endif

  delete cell_memory;
}

void setup() {
  cell_memory = new cell_memory_manager_t();
  program_global_env = new env_c(nullptr);
  source_manager = new source_manager_c();

  // Initialize the global cells (true, false, nil, etc)
  if (!global_cells_initialize()) {
    std::cerr << "Failed to initialize global cells" << std::endl;
    teardown();
    exit(1);
  }

  // Initialize the global runtime object
  if (!global_runtime_init(*program_global_env, *source_manager,
                           *cell_memory)) {
    std::cerr << "Failed to initialize global runtime" << std::endl;
    teardown();
    exit(1);
  }
}

void run_from_file(const std::string &file_name) {

  // List builder that will build lists from parsed tokens
  // and pass lists to a runtime
  list_builder_c list_builder(*cell_memory, *global_runtime);

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
