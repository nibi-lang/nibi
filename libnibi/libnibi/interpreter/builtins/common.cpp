
#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "libnibi/cell.hpp"

#include "common/platform.hpp"
#include "interpreter/builtins/cpp_macros.hpp"

namespace nibi {
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
    return allocate_cell((int64_t)(target_list->to_string().size()));
  }

  auto &list_info = target_list->as_list_info();
  return allocate_cell((int64_t)list_info.list.size());
}

cell_ptr builtin_fn_common_yield(cell_list_t &list, env_c &env) {

  if (list.size() == 1) {
    global_interpreter->set_yield_value(allocate_cell((int64_t)0));
    return global_interpreter->get_yield_value();
  }

  LIST_ENFORCE_SIZE("<-", ==, 2)

  auto target = list_get_nth_arg(1, list, env);
  global_interpreter->set_yield_value(target);
  return target;
}

cell_ptr builtin_fn_common_loop(cell_list_t &list, env_c &env) {
  // (loop (pre) (cond) (post) (body))
  LIST_ENFORCE_SIZE("loop", ==, 5)

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

  cell_ptr result = allocate_cell(cell_type_e::NIL);
  while (true) {
    auto condition_result =
        global_interpreter->execute_cell(condition, loop_env);

    if (condition_result->to_integer() <= 0) {
      return result;
    }

    result = global_interpreter->execute_cell(body, loop_env, true);

    if (global_interpreter->is_yielding()) {
      return global_interpreter->get_yield_value();
    }

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

  return allocate_cell((int64_t)0);
}

cell_ptr builtin_fn_common_put(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("put", >=, 2)

  auto it = list.begin();
  std::advance(it, 1);

  while (it != list.end()) {
    std::cout << global_interpreter->execute_cell((*it), env)->to_string();
    std::advance(it, 1);
  }
  return allocate_cell((int64_t)0);
}

cell_ptr builtin_fn_common_putln(cell_list_t &list, env_c &env) {

  if (list.size() == 1) {
    std::cout << std::endl;
    return allocate_cell((int64_t)0);
  }

  LIST_ENFORCE_SIZE("putln", >=, 2)
  auto result = builtin_fn_common_put(list, env);
  std::cout << std::endl;
  return result;
}

cell_ptr builtin_fn_common_import(cell_list_t &list, env_c &env) {

  LIST_ENFORCE_SIZE("import", >=, 2)

  auto it = list.begin();
  std::advance(it, 1);

  auto &gsm = global_interpreter->get_source_manager();

  list_builder_c list_builder(*global_interpreter);
  file_reader_c file_reader(list_builder, gsm);

  while (it != list.end()) {
    auto target = (*it)->as_string();
    auto item = global_platform->locate_file(target);
    if (!item.has_value()) {
      global_interpreter->halt_with_error(error_c(
          (*it)->locator, "Could not locate file for import: " + target));
    }
    if (!gsm.exists((*item).string())) {
      file_reader.read_file((*item).string());
    }
    std::advance(it, 1);
  }
  return allocate_cell((int64_t)0);
}
} // namespace builtins
}