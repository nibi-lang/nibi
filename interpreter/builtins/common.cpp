
#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "libnibi/cell.hpp"

#include "interpreter/builtins/cpp_macros.hpp"

namespace builtins {

cell_ptr builtin_fn_common_clone(cell_list_t &list, env_c &env) {

  LIST_ENFORCE_SIZE("clone", ==, 2)

  auto it = list.begin();
  std::advance(it, 1);

  auto loaded_cell = global_interpreter->execute_cell(*it, env);

  return loaded_cell->clone();
}

cell_ptr builtin_fn_common_len(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("len", ==, 2)

  auto target_list = list_get_nth_arg(1, list, env);

  if (target_list->type != cell_type_e::LIST) {
    return ALLOCATE_CELL((int64_t)(target_list->to_string().size()));
  }

  auto &list_info = target_list->as_list_info();
  return ALLOCATE_CELL((int64_t)list_info.list.size());
}

cell_ptr builtin_fn_common_yield(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("<-", ==, 2)

  auto target = list_get_nth_arg(1, list, env);
  global_interpreter->yield_process_with_value(target);
  return target;
}

cell_ptr builtin_fn_common_loop(cell_list_t &list, env_c &env) {
  // (loop (pre) (cond) (post) (body))
  LIST_ENFORCE_SIZE("loop", ==, 5)

  std::cout << "loop" << std::endl;

  auto it = list.begin();
  std::advance(it, 1);

  auto pre_condition = (*it);
  std::advance(it, 1);

  auto condition = (*it);
  std::advance(it, 1);

  auto post_condition = (*it);
  std::advance(it, 1);

  auto body = (*it);

  auto loop_env = env_c(&env);

  global_interpreter->execute_cell(pre_condition, loop_env);

  cell_ptr result = ALLOCATE_CELL(cell_type_e::NIL);
  while (true) {
    auto condition_result = global_interpreter->execute_cell(condition, loop_env);

    if (condition_result->to_integer() <= 0) {
      return result;
    }

    result = global_interpreter->execute_cell(body, loop_env, true);

    global_interpreter->execute_cell(post_condition, loop_env);
  }

  return result;
}

cell_ptr builtin_fn_common_if(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("?", >=, 3)

  auto it = list.begin();
  std::advance(it, 1);

  auto condition = (*it);
  std::advance(it, 1);

  auto true_condition = (*it);

  auto if_env = env_c(&env);

  auto condition_result = global_interpreter->execute_cell(condition, if_env);

  if (condition_result->as_integer() > 0) {
    return global_interpreter->execute_cell(true_condition, if_env, true);
  }

  if (list.size() == 4) {
    std::advance(it, 1);
    return global_interpreter->execute_cell((*it), if_env, true);
  }

  return ALLOCATE_CELL((int64_t)0);
}

cell_ptr builtin_fn_common_put(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("put", >=, 2)

  auto it = list.begin();
  std::advance(it, 1);

  while(it != list.end()) {
    if (it != list.begin()) {
      std::cout << " ";
    }
    std::cout << global_interpreter->execute_cell((*it), env)->to_string();
    std::advance(it, 1);
  }
  return ALLOCATE_CELL((int64_t)0);
}

cell_ptr builtin_fn_common_putln(cell_list_t &list, env_c &env) {

  if (list.size() == 1) {
    std::cout << std::endl;
    return ALLOCATE_CELL((int64_t)0);
  }

  LIST_ENFORCE_SIZE("putln", >=, 2)
  auto result = builtin_fn_common_put(list, env);
  std::cout << std::endl;
  return result;
}

} // namespace builtins
