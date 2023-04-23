#include <iostream>

#include "runtime/builtins/builtins.hpp"
#include "runtime/cell.hpp"

namespace builtins {

cell_c* builtin_fn_env_assignment(cell_list_t& list, env_c &env) {

  // TODO:  Implement this function

  std::cout << "perform assignment " << list.size() << " arguments" << std::endl;
  return global_cell_nil;
}

cell_c* builtin_fn_env_drop(cell_list_t& list, env_c &env) {

  // TODO:  Implement this function

  std::cout << "perform drop " << list.size() << " arguments" << std::endl;
  return global_cell_nil;
}

}
