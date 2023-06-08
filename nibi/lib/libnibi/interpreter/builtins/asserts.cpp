#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/keywords.hpp"

#include "macros.hpp"

namespace nibi {

namespace builtins {

cell_ptr builtin_fn_assert_true(interpreter_c &ci, cell_list_t &list,
                                env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::ASSERT, >=, 2)

  auto value = ci.execute_cell(list[1], env);
  if (value->type != cell_type_e::INTEGER) {
    throw interpreter_c::exception_c(
        "Expected item to evaluate to integer type", value->locator);
  }

  if (list.size() == 2) {
    if (value->as_integer() == 0) {
      throw interpreter_c::exception_c("Assertion failed", value->locator);
    }
    return allocate_cell(cell_type_e::NIL);
  }

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::ASSERT, ==, 3)

  if (value->as_integer() == 0) {
    auto message = ci.execute_cell(list[2], env);
    if (message->type != cell_type_e::STRING) {
      throw interpreter_c::exception_c(
          "Expected string value for assertion message", message->locator);
    }
    throw interpreter_c::exception_c(message->as_string(), value->locator);
  }

  return allocate_cell(cell_type_e::NIL);
}

cell_ptr builtin_fn_assert_eq(interpreter_c &ci, cell_list_t &list,
                              env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::ASSERT_EQ, ==, 3)

  auto lhs = ci.execute_cell(list[1], env);
  auto rhs = ci.execute_cell(list[2], env);

  if (lhs->type != rhs->type) {
    std::string err = "Expected types to be equal, but got (lhs) `";
    err += cell_type_to_string(lhs->type);
    err += "` and (rhs) `";
    err += cell_type_to_string(rhs->type);
    err += "`";
    throw interpreter_c::exception_c(err, list[0]->locator);
  }

  auto lhs_as_string = lhs->to_string();
  auto rhs_as_string = rhs->to_string();

  if (lhs_as_string != rhs_as_string) {
    std::string err = "Expected values to be equal, but got (lhs) `";
    err += lhs_as_string;
    err += "` and (rhs) `";
    err += rhs_as_string;
    err += "`";
    throw interpreter_c::exception_c(err, list[0]->locator);
  }

  return allocate_cell(cell_type_e::NIL);
}

cell_ptr builtin_fn_assert_neq(interpreter_c &ci, cell_list_t &list,
                               env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::ASSERT_NEQ, ==, 3)

  auto lhs = ci.execute_cell(list[1], env)->to_string();
  auto rhs = ci.execute_cell(list[2], env)->to_string();

  if (lhs == rhs) {
    std::string err = "Expected values to be not equal, but got (lhs) `";
    err += lhs;
    err += "` and (rhs) `";
    err += rhs;
    err += "`";
    throw interpreter_c::exception_c(err, list[0]->locator);
  }

  return allocate_cell(cell_type_e::NIL);
}

} // namespace builtins

} // namespace nibi
