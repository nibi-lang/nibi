#include "lib.hpp"

#include <string>
#include <iostream>

nibi::cell_ptr get_str(nibi::cell_list_t &list, nibi::env_c &env) {
  std::string line;
  std::getline(std::cin, line);
  return nibi::allocate_cell(line);
}
