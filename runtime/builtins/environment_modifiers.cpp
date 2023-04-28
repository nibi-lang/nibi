#include <iostream>

#include "runtime/builtins/builtins.hpp"
#include "runtime/cell.hpp"
#include "runtime/runtime.hpp"

#include "cpp_macros.hpp"

#include <iterator>
namespace builtins {

cell_c *builtin_fn_env_assignment(cell_list_t &list, env_c &env) {

  LIST_ENFORCE_SIZE(":=", ==, 3)

  auto it = list.begin();
  std::advance(it, 1);

  if ((*it)->type != cell_type_e::SYMBOL) {
    throw runtime_c::exception_c("Expected symbol as first argument to :=",
                                 (*it)->locator);
  }

  auto &target_variable_name = (*it)->as_string();

  // Ensure that the user doesn't attempt to overwrite a temporary variable

  if (target_variable_name.starts_with('$')) {
    throw runtime_c::exception_c(
        "Cannot assign to a variable that starts with a '$' as those are "
        "reserved for temporary variables",
        (*it)->locator);
  }

  auto *target_assignment_value =
      global_runtime->execute_cell(list_get_nth_arg(2, list, env), env);

  // Explicitly clone the value as we might be reading from
  // an instruction that will be mutated later

  target_assignment_value = target_assignment_value->clone();

  // Ensure that the user doesn't attempt to overwrite an existing
  // item without freeing it frist
  {
    auto &current_env_map = env.get_map();
    if (current_env_map.find(target_variable_name) != current_env_map.end()) {
      current_env_map[target_variable_name]->mark_as_in_use(false);
      current_env_map.erase(target_variable_name);
    }
  }

  env.set(target_variable_name, *target_assignment_value);

  // Return a pointer to the new cell so assignments can be chained
  return target_assignment_value;
}

cell_c *builtin_fn_env_set(cell_list_t &list, env_c &env) {

  LIST_ENFORCE_SIZE("set", ==, 3)

  auto *target_assignment_cell =
      global_runtime->execute_cell(list_get_nth_arg(1, list, env), env);

  if (target_assignment_cell == global_cell_nil ||
      target_assignment_cell == global_cell_true ||
      target_assignment_cell == global_cell_false) {
    throw runtime_c::exception_c(
        "Resulting cell invalid for `set` operation: " +
            target_assignment_cell->to_string(),
        list.front()->locator);
  }

  auto *target_assignment_value =
      global_runtime->execute_cell(list_get_nth_arg(2, list, env), env);

  // Then update that cell directly
  target_assignment_cell->update_data_and_type_to(*target_assignment_value);

  return target_assignment_cell;
}

cell_c *builtin_fn_env_drop(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("drop", >=, 2)
  LIST_ITER_SKIP_N(1, {
    if (!env.drop((*it)->as_symbol())) {
      throw runtime_c::exception_c("Could not find symbol with name :" +
                                       (*it)->as_symbol(),
                                   (*it)->locator);
    }
  })
  return global_cell_nil;
}

} // namespace builtins
