#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "libnibi/cell.hpp"

#include "cpp_macros.hpp"

namespace builtins {

cell_ptr builtin_fn_assert_true(cell_list_t &list, env_c &env) {

  LIST_ENFORCE_SIZE("assert", ==, 3)

  auto value = list_get_nth_arg(1, list, env);
  if (value->type != cell_type_e::INTEGER) {
    throw interpreter_c::exception_c("Expected item to evaluate to integer type",
                                 value->locator);
  }

  if (value->as_integer() == 0) {
    auto message = list_get_nth_arg(2, list, env);
    if (message->type != cell_type_e::STRING) {
      throw interpreter_c::exception_c(
          "Expected string value for assertion message", message->locator);
    }
    throw interpreter_c::exception_c(message->as_string(), value->locator);
  }

  return global_cell_nil;
}

} // namespace builtins
