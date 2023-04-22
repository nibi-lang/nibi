#include "runtime.hpp"

#include <iostream>

runtime_c::runtime_c(env_c &global_env, source_manager_c& source_manager, cell_memory_manager_t& instruction_memory)
  : global_env_(global_env),
    source_manager_(source_manager),
    instruction_memory_(instruction_memory),
    runtime_memory_(&global_env_) {}

void runtime_c::on_list(cell_c* list_cell) {
  std::cout << "Runtime got list: " << list_cell->to_string() << std::endl;

  // Process the list 

  // Mark as no longer in use

  // Every so often the runtime should sweep instruction_memory_ 
  // beacuse all instruction cells will be allocated by the list builder
  // in the instruction memory manager and will be marked as in use
  // once the instruction is executed it will be marked as no longer in use
  // and the runtime will sweep the instruction memory to free up cells

  // Profit
}
