#include "lib.hpp"

#include <string>
#include <iostream>
#include <memory>

nibi::cell_ptr test(nibi::cell_list_t &list, nibi::env_c &env) {
  return nibi::allocate_cell((int64_t)1);
}
