#include <iostream>

#include "arithmetic_helpers.hpp"
#include "interpreter/builtins/builtins.hpp"
#include "interpreter/builtins/cpp_macros.hpp"
#include "libnibi/cell.hpp"
#include "interpreter/interpreter.hpp"

namespace builtins {

cell_ptr builtin_fn_bitwise_lsh(cell_list_t &list, env_c &env){
  LIST_ENFORCE_SIZE("lsh", ==, 3)
  auto lhs = list_get_nth_arg(1, list, env)->to_integer();
  auto rhs = list_get_nth_arg(2, list, env)->to_integer();
  return ALLOCATE_CELL((int64_t)(lhs << rhs));
}

cell_ptr builtin_fn_bitwise_rsh(cell_list_t &list, env_c &env){
  LIST_ENFORCE_SIZE("rsh", ==, 3)
  auto lhs = list_get_nth_arg(1, list, env)->to_integer();
  auto rhs = list_get_nth_arg(2, list, env)->to_integer();
  return ALLOCATE_CELL((int64_t)(lhs >> rhs));
}


} // namespace builtins