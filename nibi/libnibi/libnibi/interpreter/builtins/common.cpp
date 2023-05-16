
#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "libnibi/cell.hpp"

#include "common/platform.hpp"
#include "interpreter/builtins/cpp_macros.hpp"
#include "interpreter/interpreter.hpp"

namespace nibi {
namespace builtins {

cell_ptr builtin_fn_common_clone(interpreter_c &ci, cell_list_t &list,
                                 env_c &env) {

  LIST_ENFORCE_SIZE("clone", ==, 2)

  auto it = list.begin();
  std::advance(it, 1);

  auto loaded_cell = ci.execute_cell(*it, env);

  return loaded_cell->clone();
}

cell_ptr builtin_fn_common_len(interpreter_c &ci, cell_list_t &list,
                               env_c &env) {
  LIST_ENFORCE_SIZE("len", ==, 2)

  auto target_list = list_get_nth_arg(ci, 1, list, env);

  if (target_list->type != cell_type_e::LIST) {
    return allocate_cell((int64_t)(target_list->to_string(false).size()));
  }

  auto &list_info = target_list->as_list_info();
  return allocate_cell((int64_t)list_info.list.size());
}

cell_ptr builtin_fn_common_yield(interpreter_c &ci, cell_list_t &list,
                                 env_c &env) {

  if (list.size() == 1) {
    ci.set_yield_value(allocate_cell((int64_t)0));
    return ci.get_yield_value();
  }

  LIST_ENFORCE_SIZE("<-", ==, 2)

  auto target = list_get_nth_arg(ci, 1, list, env);
  ci.set_yield_value(target);
  return target;
}

cell_ptr builtin_fn_common_loop(interpreter_c &ci, cell_list_t &list,
                                env_c &env) {
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

  ci.execute_cell(pre_condition, loop_env);

  cell_ptr result = allocate_cell(cell_type_e::NIL);
  while (true) {
    auto condition_result = ci.execute_cell(condition, loop_env);

    if (condition_result->to_integer() <= 0) {
      return result;
    }

    result = ci.execute_cell(body, loop_env, true);

    if (ci.is_yielding()) {
      return ci.get_yield_value();
    }

    ci.execute_cell(post_condition, loop_env);
  }

  return result;
}

cell_ptr builtin_fn_common_if(interpreter_c &ci, cell_list_t &list,
                              env_c &env) {
  LIST_ENFORCE_SIZE("?", >=, 3)

  auto it = list.begin();
  std::advance(it, 1);

  auto condition = (*it);
  std::advance(it, 1);

  auto true_condition = (*it);

  auto if_env = env_c(&env);

  auto condition_result = ci.execute_cell(condition, if_env);

  if (condition_result->as_integer() > 0) {
    return ci.execute_cell(true_condition, if_env, true);
  }

  if (list.size() == 4) {
    std::advance(it, 1);
    return ci.execute_cell((*it), if_env, true);
  }

  return allocate_cell((int64_t)0);
}

cell_ptr builtin_fn_common_put(interpreter_c &ci, cell_list_t &list,
                               env_c &env) {
  LIST_ENFORCE_SIZE("put", >=, 2)

  auto it = list.begin();
  std::advance(it, 1);

  while (it != list.end()) {
    std::cout << ci.execute_cell((*it), env)->to_string();
    std::advance(it, 1);
  }
  return allocate_cell((int64_t)0);
}

cell_ptr builtin_fn_common_putln(interpreter_c &ci, cell_list_t &list,
                                 env_c &env) {

  if (list.size() == 1) {
    std::cout << std::endl;
    return allocate_cell((int64_t)0);
  }

  LIST_ENFORCE_SIZE("putln", >=, 2)
  auto result = builtin_fn_common_put(ci, list, env);
  std::cout << std::endl;
  return result;
}

cell_ptr builtin_fn_common_import(interpreter_c &ci, cell_list_t &list,
                                  env_c &env) {

  LIST_ENFORCE_SIZE("import", >=, 2)

  auto it = list.begin();
  std::advance(it, 1);

  auto &gsm = ci.get_source_manager();

  list_builder_c list_builder(ci);
  file_reader_c file_reader(list_builder, gsm);

  while (it != list.end()) {
    // Get the source file from the `import` keyword so we can ensure that
    // the file can be searched relative to the location of the file being
    // executed at the moment
    auto from =
        std::filesystem::path((*list.begin())->locator->get_source_name());

    // Retrieve the file name to import
    auto target = std::filesystem::path((*it)->as_string());

    // Locate the item
    auto item = global_platform->locate_file(target, from);
    if (!item.has_value()) {
      ci.halt_with_error(
          error_c((*it)->locator,
                  "Could not locate file for import: " + target.string()));
    }

    // Check that the item hasn't already been imported
    if (!gsm.exists((*item).string())) {
      file_reader.read_file((*item).string());
    }
    std::advance(it, 1);
  }
  return allocate_cell((int64_t)1);
}

cell_ptr builtin_fn_common_use(interpreter_c &ci, cell_list_t &list,
                               env_c &env) {

  LIST_ENFORCE_SIZE("use", >=, 2)

  auto it = list.begin();
  std::advance(it, 1);

  while (it != list.end()) {
    ci.load_module((*it));
    std::advance(it, 1);
  }
  return allocate_cell((int64_t)1);
}

cell_ptr builtin_fn_common_exit(interpreter_c &ci, cell_list_t &list,
                                env_c &env) {
  LIST_ENFORCE_SIZE("exit", ==, 2)
  auto it = list.begin();
  std::advance(it, 1);
  std::exit(ci.execute_cell((*it), env)->as_integer());
}

} // namespace builtins
} // namespace nibi