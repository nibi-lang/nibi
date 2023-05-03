#include <iostream>

#include "runtime/builtins/builtins.hpp"
#include "runtime/cell.hpp"

#include "runtime/builtins/cpp_macros.hpp"

namespace builtins {

cell_ptr builtin_fn_check_nil(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("nil?", ==, 2)
  auto first_arg = list_get_nth_arg(1, list, env);
  if (first_arg->type == cell_type_e::NIL) {
    return global_cell_true;
  }
  return global_cell_false;
}

} // namespace builtins
