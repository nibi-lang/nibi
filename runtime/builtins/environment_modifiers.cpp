#include <iostream>

#include "runtime/builtins/builtins.hpp"
#include "runtime/cell.hpp"

#include "cpp_macros.hpp"

#include <iterator>
namespace builtins {

cell_c *builtin_fn_env_assignment(cell_list_t &list, env_c &env) {

  LIST_ENFORCE_SIZE(==, 3)

  auto it = list.begin();
  std::advance(it, 1);

  auto& target_variable_name = (*it)->as_string();

  auto target_assignment_value = global_runtime->execute_cell(
    list_get_nth_arg(2, list, env), 
    env);

  auto target_env = env.get_env(target_variable_name);

  if (!target_env) {
    target_env = &env;
  }

  target_env->set_local_cell(target_variable_name, *target_assignment_value);

  // Return a pointer to the new cell so assignments can be chained
  return target_assignment_value;
}

cell_c *builtin_fn_env_drop(cell_list_t &list, env_c &env) {

  // TODO:  Implement this function

  std::cout << "perform drop " << list.size() << " arguments" << std::endl;
  return global_cell_nil;
}

} // namespace builtins
