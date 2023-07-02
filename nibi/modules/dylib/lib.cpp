#include "lib.hpp"

#include <iostream>
#include <memory>
#include <string>

#include <libnibi/macros.hpp>
#include <libnibi/nibi.hpp>

nibi::cell_ptr _dylib_create_rll(nibi::cell_processor_if &ci, nibi::cell_list_t &list,
                        nibi::env_c &env) {
  //  NIBI_LIST_ENFORCE_SIZE("{cffi load_lib}", >, 1)
  

  std::cout << "Create RLL" << std::endl;

  return nibi::allocate_cell(nibi::cell_type_e::NIL);
}

nibi::cell_ptr _dylib_bind_fn(nibi::cell_processor_if &ci, nibi::cell_list_t &list,
                        nibi::env_c &env) {
  //  NIBI_LIST_ENFORCE_SIZE("{cffi load_lib}", >, 1)


  std::cout << "Bind FN" << std::endl;

  return nibi::allocate_cell(nibi::cell_type_e::NIL);
}
