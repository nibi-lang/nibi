#include "lib.hpp"

#include <iostream>
#include <memory>
#include <string>

nibi::cell_ptr get_str(nibi::cell_list_t &list, nibi::env_c &env) {
  std::string line;
  std::getline(std::cin, line);
  return nibi::allocate_cell(line);
}
