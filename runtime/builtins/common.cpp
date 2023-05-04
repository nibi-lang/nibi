
#include <iostream>

#include "runtime/builtins/builtins.hpp"
#include "runtime/cell.hpp"

#include "runtime/builtins/cpp_macros.hpp"

namespace builtins {

cell_ptr builtin_fn_common_len(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("len", ==, 2)

  auto target_list = list_get_nth_arg(1, list, env);

  if (target_list->type != cell_type_e::LIST) {
    return std::make_shared<cell_c>((int64_t)(target_list->to_string().size()));
  }

  auto &list_info = target_list->as_list_info();
  return std::make_shared<cell_c>((int64_t)list_info.list.size());
}

cell_ptr builtin_fn_common_yield(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("<-", ==, 2)

  auto target = list_get_nth_arg(1, list, env);
  global_runtime->yield_process_with_value(target);
  return target;
}

} // namespace builtins
