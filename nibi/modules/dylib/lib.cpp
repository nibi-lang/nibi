#include "lib.hpp"

#include <iostream>
#include <memory>
#include <string>

#include <libnibi/macros.hpp>
#include <libnibi/nibi.hpp>

nibi::cell_ptr load_lib(nibi::cell_processor_if &ci, nibi::cell_list_t &list,
                        nibi::env_c &env) {
  //  NIBI_LIST_ENFORCE_SIZE("{cffi load_lib}", >, 1)

  return nibi::allocate_cell(nibi::cell_type_e::NIL);
}
