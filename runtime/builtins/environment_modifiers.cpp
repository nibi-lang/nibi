#include <iostream>

#include "runtime/builtins/builtins.hpp"
#include "runtime/cell.hpp"

namespace builtins {

cell_c* builtin_fn_env_assignment(cell_list_t args, env_c &env) {

  // TODO:  Implement this function

  std::cout << "perform assignment " << args.size() << " arguments" << std::endl;
  return nullptr;
}

cell_c* builtin_fn_env_drop(cell_list_t args, env_c &env) {

  // TODO:  Implement this function

  std::cout << "perform drop " << args.size() << " arguments" << std::endl;
  return nullptr;
}

}
