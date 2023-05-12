#include <iostream>

#include "arithmetic_helpers.hpp"
#include "interpreter/builtins/builtins.hpp"
#include "interpreter/builtins/cpp_macros.hpp"
#include "interpreter/interpreter.hpp"
#include "libnibi/cell.hpp"

namespace nibi {
namespace builtins {

cell_ptr builtin_fn_bitwise_lsh(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("bw-lsh", ==, 3)
  auto lhs = list_get_nth_arg(1, list, env)->to_integer();
  auto rhs = list_get_nth_arg(2, list, env)->to_integer();
  return allocate_cell((int64_t)(lhs << rhs));
}

cell_ptr builtin_fn_bitwise_rsh(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("bw-rsh", ==, 3)
  auto lhs = list_get_nth_arg(1, list, env)->to_integer();
  auto rhs = list_get_nth_arg(2, list, env)->to_integer();
  return allocate_cell((int64_t)(lhs >> rhs));
}

cell_ptr builtin_fn_bitwise_and(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("bw-and", ==, 3)
  auto lhs = list_get_nth_arg(1, list, env)->to_integer();
  auto rhs = list_get_nth_arg(2, list, env)->to_integer();
  return allocate_cell((int64_t)(lhs & rhs));
}

cell_ptr builtin_fn_bitwise_or(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("bw-or", ==, 3)
  auto lhs = list_get_nth_arg(1, list, env)->to_integer();
  auto rhs = list_get_nth_arg(2, list, env)->to_integer();
  return allocate_cell((int64_t)(lhs | rhs));
}

cell_ptr builtin_fn_bitwise_xor(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("bw-xor", ==, 3)
  auto lhs = list_get_nth_arg(1, list, env)->to_integer();
  auto rhs = list_get_nth_arg(2, list, env)->to_integer();
  return allocate_cell((int64_t)(lhs ^ rhs));
}

cell_ptr builtin_fn_bitwise_not(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("bw-not", ==, 2)
  auto lhs = list_get_nth_arg(1, list, env)->to_integer();
  return allocate_cell((int64_t)(~lhs));
}

} // namespace builtins
} // namespace nibi