#include "lib.hpp"

#include <iostream>
#include <memory>
#include <string>

nibi::cell_ptr example_function(nibi::cell_list_t &list, nibi::env_c &env) {
  std::string out = "HEY! I'm a C++ function!\n";
  return nibi::allocate_cell(out);
}

void nibi_runtime_init() {
  std::cout << "Module initialized!" << std::endl;
}

void nibi_runtime_deinit() {
  std::cout << "Module shutting down!" << std::endl;
}
