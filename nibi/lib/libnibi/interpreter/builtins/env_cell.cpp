#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "libnibi/cell.hpp"

#include "macros.hpp"

namespace nibi {

namespace builtins {

cell_ptr builtin_fn_envcell_env(interpreter_c &ci, cell_list_t &list,
                                env_c &env) {

  NIBI_LIST_ENFORCE_SIZE("env", ==, 3)

  auto it = list.begin();
  std::advance(it, 1);

  if ((*it)->type != cell_type_e::SYMBOL) {
    throw interpreter_c::exception_c(
        "env expects first item to be a name for the environment ",
        (*it)->locator);
  }

  auto env_name = (*it)->as_symbol();

  environment_info_s new_env_info{env_name, new env_c(env)};

  // Populate the env by executing their instructions
  std::advance(it, 1);
  ci.execute_cell((*it), *new_env_info.env, true);

  // Create a cell that will hold the env we just built
  auto new_cell = allocate_cell(new_env_info);
  env.set(env_name, new_cell);

  return new_cell;
}

} // namespace builtins
} // namespace nibi
