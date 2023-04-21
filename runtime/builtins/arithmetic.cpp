#include <iostream>

#include "runtime/builtins/builtins.hpp"
#include "runtime/cell.hpp"

namespace builtins {




cell_c *builtin_fn_arithmetic_add(cell_list_t args, env_c &env) {


  // TODO:  Implement this function

  std::cout << "peform add on " << args.size() << " arguments" << std::endl;
  return nullptr;
}

}