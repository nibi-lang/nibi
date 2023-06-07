#include <iostream>

#include "arithmetic_helpers.hpp"
#include "interpreter/builtins/builtins.hpp"
#include "interpreter/interpreter.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/keywords.hpp"
#include "macros.hpp"

namespace nibi {
namespace builtins {

cell_ptr builtin_fn_bitwise_lsh(interpreter_c &ci, cell_list_t &list,
                                env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::BW_LSH, ==, 3)
  auto lhs = list_get_nth_arg(ci, 1, list, env)->to_integer();
  auto rhs = list_get_nth_arg(ci, 2, list, env)->to_integer();
  return allocate_cell((int64_t)(lhs << rhs));
}

cell_ptr builtin_fn_bitwise_rsh(interpreter_c &ci, cell_list_t &list,
                                env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::BW_RSH, ==, 3)
  auto lhs = list_get_nth_arg(ci, 1, list, env)->to_integer();
  auto rhs = list_get_nth_arg(ci, 2, list, env)->to_integer();
  return allocate_cell((int64_t)(lhs >> rhs));
}

cell_ptr builtin_fn_bitwise_and(interpreter_c &ci, cell_list_t &list,
                                env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::BW_AND, ==, 3)
  auto lhs = list_get_nth_arg(ci, 1, list, env)->to_integer();
  auto rhs = list_get_nth_arg(ci, 2, list, env)->to_integer();
  return allocate_cell((int64_t)(lhs & rhs));
}

cell_ptr builtin_fn_bitwise_or(interpreter_c &ci, cell_list_t &list,
                               env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::BW_OR, ==, 3)
  auto lhs = list_get_nth_arg(ci, 1, list, env)->to_integer();
  auto rhs = list_get_nth_arg(ci, 2, list, env)->to_integer();
  return allocate_cell((int64_t)(lhs | rhs));
}

cell_ptr builtin_fn_bitwise_xor(interpreter_c &ci, cell_list_t &list,
                                env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::BW_XOR, ==, 3)
  auto lhs = list_get_nth_arg(ci, 1, list, env)->to_integer();
  auto rhs = list_get_nth_arg(ci, 2, list, env)->to_integer();
  return allocate_cell((int64_t)(lhs ^ rhs));
}

cell_ptr builtin_fn_bitwise_not(interpreter_c &ci, cell_list_t &list,
                                env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::BW_NOT, ==, 2)
  auto lhs = list_get_nth_arg(ci, 1, list, env)->to_integer();
  return allocate_cell((int64_t)(~lhs));
}

} // namespace builtins
} // namespace nibi
