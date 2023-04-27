
#include <iostream>

#include "runtime/builtins/builtins.hpp"
#include "runtime/cell.hpp"

#include "runtime/builtins/cpp_macros.hpp"

namespace builtins {

cell_c *builtin_fn_common_len(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("len", ==, 2)

  auto *target_list = list_get_nth_arg(1, list, env);

  if (target_list->type != cell_type_e::LIST) {
    return global_runtime->get_runtime_memory().allocate(
        (int64_t)(target_list->to_string().size()));
  }

  auto &list_info = target_list->as_list_info();
  return global_runtime->get_runtime_memory().allocate(
      (int64_t)list_info.list.size());
}

} // namespace builtins
