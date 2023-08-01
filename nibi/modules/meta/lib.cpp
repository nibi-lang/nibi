#include "lib.hpp"

#include <iostream>
#include <libnibi/macros.hpp>

nibi::cell_ptr meta_cell(nibi::cell_processor_if &ci, nibi::cell_list_t &list,
                         nibi::env_c &env) {
  return nibi::allocate_cell((int64_t)sizeof(nibi::cell_c));
}

nibi::cell_ptr meta_locator(nibi::cell_processor_if &ci,
                            nibi::cell_list_t &list, nibi::env_c &env) {
  return nibi::allocate_cell((int64_t)sizeof(nibi::locator_ptr));
}
