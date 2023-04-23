#include <iostream>

#include "runtime/builtins/builtins.hpp"
#include "runtime/cell.hpp"

namespace builtins {

cell_c *builtin_fn_assert_true(cell_list_t &list, env_c &env) {

  // TODO:  Implement this function

  std::cout << "true! on " << list.size() << " arguments" << std::endl;
  return global_cell_nil;
}

} // namespace builtins
