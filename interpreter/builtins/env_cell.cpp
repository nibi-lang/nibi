#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "libnibi/cell.hpp"

#include "cpp_macros.hpp"

namespace builtins {

cell_ptr builtin_fn_envcell_env(cell_list_t &list, env_c &env) {

  LIST_ENFORCE_SIZE("env", ==, 3)

  auto it = list.begin();
  std::advance(it, 1);

  auto env_name = (*it)->as_symbol();

  environment_info_s new_env_info{env_name, new env_c(env)};

  // The `priv` env to be populated within the cells env for 
  // local "private" things
  auto env_map = new_env_info.env->get_map();
  env_map["priv"] = ALLOCATE_CELL(environment_info_s{"priv", new env_c()});

  // Populate the env by executing their instructions
  std::advance(it, 1);
  global_interpreter->execute_cell((*it), *new_env_info.env, true);

  // Create a cell that will hold the env we just built
  auto new_cell = ALLOCATE_CELL(new_env_info);
  env.set(env_name, new_cell);

  return new_cell;
}

} // namespace builtins
