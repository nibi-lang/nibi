#include <iostream>

#include "runtime/builtins/builtins.hpp"
#include "runtime/cell.hpp"

namespace builtins {

cell_c *builtin_fn_arithmetic_add(cell_list_t args, env_c &env) {

  // TODO:  Implement this function

  std::cout << "peform add on " << args.size() << " arguments" << std::endl;
  return nullptr;
}

cell_c *builtin_fn_arithmetic_sub(cell_list_t args, env_c &env) {

  // TODO:  Implement this function

  std::cout << "peform sub on " << args.size() << " arguments" << std::endl;
  return nullptr;
}

cell_c *builtin_fn_arithmetic_div(cell_list_t args, env_c &env) {

  // TODO:  Implement this function

  std::cout << "peform div on " << args.size() << " arguments" << std::endl;
  return nullptr;
}

cell_c *builtin_fn_arithmetic_mul(cell_list_t args, env_c &env) {

  // TODO:  Implement this function

  std::cout << "peform mul on " << args.size() << " arguments" << std::endl;
  return nullptr;
}

cell_c *builtin_fn_arithmetic_mod(cell_list_t args, env_c &env) {

  // TODO:  Implement this function

  std::cout << "peform mod on " << args.size() << " arguments" << std::endl;
  return nullptr;
}

cell_c *builtin_fn_arithmetic_pow(cell_list_t args, env_c &env) {

  // TODO:  Implement this function

  std::cout << "peform pow on " << args.size() << " arguments" << std::endl;
  return nullptr;
}

}