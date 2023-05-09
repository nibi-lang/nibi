#include "lib.hpp"

#include <string>
#include <iostream>

cell_ptr get_str(cell_list_t &list, env_c &env) {
  std::string line;
  std::getline(std::cin, line);
  return ALLOCATE_CELL(line);
}
