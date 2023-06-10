#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "keywords.hpp"
#include "libnibi/cell.hpp"
#include "macros.hpp"

namespace nibi {
namespace builtins {

cell_ptr builtin_fn_list_push_front(cell_processor_if &ci, cell_list_t &list,
                                    env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::PUSH_FRONT, ==, 3)

  auto value_to_push = ci.process_cell(list[2], env);

  auto list_to_push_to = ci.process_cell(list[1], env);

  auto &list_info = list_to_push_to->as_list_info();

  // Clone the target and push it back
  list_info.list.push_front(value_to_push->clone(env));

  return list_to_push_to;
}

cell_ptr builtin_fn_list_push_back(cell_processor_if &ci, cell_list_t &list,
                                   env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::PUSH_BACK, ==, 3)

  auto value_to_push = ci.process_cell(list[2], env);

  auto list_to_push_to = ci.process_cell(list[1], env);

  auto &list_info = list_to_push_to->as_list_info();

  // Clone the target and push it back
  list_info.list.push_back(value_to_push->clone(env));

  return list_to_push_to;
}

cell_ptr builtin_fn_list_pop_back(cell_processor_if &ci, cell_list_t &list,
                                  env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::POP_BACK, ==, 2)

  auto target = ci.process_cell(list[1], env);

  auto &list_info = target->as_list_info();

  if (list_info.list.empty()) {
    return target;
  }

  list_info.list.pop_back();

  return target;
}

cell_ptr builtin_fn_list_pop_front(cell_processor_if &ci, cell_list_t &list,
                                   env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::POP_FRONT, ==, 2)

  auto target = ci.process_cell(list[1], env);

  auto &list_info = target->as_list_info();

  if (list_info.list.empty()) {
    return target;
  }

  list_info.list.pop_front();

  return target;
}

cell_ptr builtin_fn_list_iter(cell_processor_if &ci, cell_list_t &list,
                              env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::ITER, ==, 4)

  auto list_to_iterate = ci.process_cell(list[1], env);

  auto &list_info = list_to_iterate->as_list_info();

  auto it = list.begin();

  std::advance(it, 2);
  auto symbol_to_bind = (*it)->as_symbol();

  std::advance(it, 1);
  auto ins_to_exec_per_item = (*it);

  auto iter_env = env_c(&env);

  auto &current_env_map = iter_env.get_map();

  for (auto cell : list_info.list) {

    current_env_map[symbol_to_bind] = ci.process_cell(cell, iter_env);

    ci.process_cell(ins_to_exec_per_item, iter_env, true);
  }

  // Return the list we iterated
  return list_to_iterate;
}

cell_ptr builtin_fn_list_at(cell_processor_if &ci, cell_list_t &list,
                            env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::AT, ==, 3)

  auto requested_idx = ci.process_cell(list[2], env);

  auto target_list = ci.process_cell(list[1], env);

  auto &list_info = target_list->as_list_info();

  auto actual_idx_val = requested_idx->as_integer();

  if (actual_idx_val < 0 || actual_idx_val >= list_info.list.size()) {
    throw interpreter_c::exception_c("Index out of bounds (OOB)",
                                     list[2]->locator);
  }

  return ci.process_cell(list_info.list[actual_idx_val], env);
}

cell_ptr builtin_fn_list_spawn(cell_processor_if &ci, cell_list_t &list,
                               env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::SPAWN, ==, 3)

  auto list_size = ci.process_cell(list[2], env);

  if (list_size->as_integer() < 0) {
    auto it = list.begin();
    std::advance(it, 2);
    throw interpreter_c::exception_c("Cannot spawn a list with a negative size",
                                     (*it)->locator);
  }

  return allocate_cell(
      list_info_s{list_types_e::DATA,
                  cell_list_t(list_size->as_integer(),
                              ci.process_cell(list[1], env)->clone(env))});
}

} // namespace builtins
} // namespace nibi
