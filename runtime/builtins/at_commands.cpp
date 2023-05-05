
#include <iostream>

#include "runtime/builtins/builtins.hpp"
#include "libnibi/cell.hpp"

#include "cpp_macros.hpp"

namespace builtins {

/*
  @debug <0|1>
*/
cell_ptr builtin_fn_at_debug(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("@debug", ==, 2)
  auto value = list_get_nth_arg(1, list, env);
  if (value->type != cell_type_e::INTEGER) {
    throw runtime_c::exception_c("Expected integer value", value->locator);
  }
  global_runtime->set_debug_enabled(value->as_integer() != 0);
  return global_cell_true;
}

} // namespace builtins
