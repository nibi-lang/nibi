#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/keywords.hpp"

#include "macros.hpp"

namespace nibi {

namespace builtins {

cell_ptr builtin_fn_assert_true(cell_processor_if &ci, cell_list_t &list,
                                env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::ASSERT, >=, 2)

  auto value = ci.process_cell(list[1], env);
  if (!value->is_integer()) {
    throw interpreter_c::exception_c(
        "Expected item to evaluate to integer type", list[1]->locator);
  }

  if (list.size() == 2) {
    if (value->as_integer() == 0) {
      throw interpreter_c::exception_c("Assertion failed", list[0]->locator);
    }
    return allocate_cell(cell_type_e::NIL);
  }

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::ASSERT, ==, 3)

  if (value->as_integer() == 0) {
    auto message = ci.process_cell(list[2], env);
    if (message->type != cell_type_e::STRING) {
      throw interpreter_c::exception_c(
          "Expected string value for assertion message", message->locator);
    }
    throw interpreter_c::exception_c(message->as_string(), list[0]->locator);
  }

  return allocate_cell(cell_type_e::NIL);
}

} // namespace builtins

} // namespace nibi
