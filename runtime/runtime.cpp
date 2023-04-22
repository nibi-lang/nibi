#include "runtime.hpp"

#include <iostream>

runtime_c::runtime_c(env_c &global_env, source_manager_c& source_manager )
  : global_env_(global_env),
    source_manager_(source_manager),
    runtime_memory_(&global_env_) {}

void runtime_c::on_list(cell_c* list_cell) {
  std::cout << "Runtime got list: " << list_cell->to_string() << std::endl;

}