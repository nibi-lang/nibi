#include <iostream>

#include "runtime/builtins/builtins.hpp"
#include "runtime/cell.hpp"
#include "runtime/builtins/cpp_macros.hpp"
#include "runtime/runtime.hpp"
#include "arithmetic_helpers.hpp"

namespace builtins {

cell_c *builtin_fn_arithmetic_add(cell_list_t& list, env_c &env) {

  LIST_ENFORCE_SIZE(>=, 2)

  auto* first_arg = list_get_nth_arg(1, list, env);

  switch (first_arg->type) {
    case cell_type_e::INTEGER: {
      return global_runtime->get_runtime_memory().allocate(
        list_perform_sum<int64_t>(first_arg->to_integer(), [](cell_c* arg) -> int64_t {
          return arg->to_integer();
        }, list, env)
      );
    }
    case cell_type_e::DOUBLE: {
      return global_runtime->get_runtime_memory().allocate(
        list_perform_sum<double>(first_arg->to_double(), [](cell_c* arg) -> double {
          return arg->to_double();
        }, list, env)
      );
    }
    default: {
      std::string msg = "Incorrect argument type for arithmetic function: ";
      msg += cell_type_to_string(first_arg->type);
      global_runtime->halt_with_error(error_c(first_arg->locator, msg));
      break;
    }
  }
  return global_cell_nil;
}

cell_c *builtin_fn_arithmetic_sub(cell_list_t& list, env_c &env) {

  // TODO:  Implement this function

  std::cout << "peform sub on " << list.size() << " arguments" << std::endl;
  return global_cell_nil;
}

cell_c *builtin_fn_arithmetic_div(cell_list_t& list, env_c &env) {

  // TODO:  Implement this function

  std::cout << "peform div on " << list.size() << " arguments" << std::endl;
  return global_cell_nil;
}

cell_c *builtin_fn_arithmetic_mul(cell_list_t& list, env_c &env) {

  // TODO:  Implement this function

  std::cout << "peform mul on " << list.size() << " arguments" << std::endl;
  return global_cell_nil;
}

cell_c *builtin_fn_arithmetic_mod(cell_list_t& list, env_c &env) {

  // TODO:  Implement this function

  std::cout << "peform mod on " << list.size() << " arguments" << std::endl;
  return global_cell_nil;
}

cell_c *builtin_fn_arithmetic_pow(cell_list_t& list, env_c &env) {

  // TODO:  Implement this function

  std::cout << "peform pow on " << list.size() << " arguments" << std::endl;
  return global_cell_nil;
}

}