#include "lib.hpp"

#include <iostream>
#include <memory>
#include <string>

nibi::cell_ptr test(nibi::cell_list_t &list, nibi::env_c &env) {
  return nibi::allocate_cell((int64_t)1);
}
