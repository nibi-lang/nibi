#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "interpreter/interpreter.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/keywords.hpp"
#include "macros.hpp"

#include <iterator>

namespace nibi {
namespace builtins {

cell_ptr builtin_fn_env_assignment(interpreter_c &ci, cell_list_t &list,
                                   env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::ASSIGN, ==, 3)

  auto it = list.begin();
  std::advance(it, 1);

  if ((*it)->type != cell_type_e::SYMBOL) {
    throw interpreter_c::exception_c(
        "Expected symbol as first argument to assign", (*it)->locator);
  }

  auto &target_variable_name = (*it)->as_string();

  if (target_variable_name[0] == '$') {
    throw interpreter_c::exception_c(
        "Cannot assign to a variable starting with '$'", (*it)->locator);
  }

  auto target_assignment_value =
      ci.process_cell(ci.process_cell(list[2], env), env);

  // Explicitly clone the value as we might be reading from
  // an instruction that will be mutated later

  target_assignment_value = target_assignment_value->clone(env);

  env.set(target_variable_name, target_assignment_value);

  // Return a pointer to the new cell so assignments can be chained
  return target_assignment_value;
}

cell_ptr builtin_fn_env_set(interpreter_c &ci, cell_list_t &list, env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::SET, ==, 3)

  auto target_assignment_cell =
      ci.process_cell(ci.process_cell(list[1], env), env);

  auto target_assignment_value =
      ci.process_cell(ci.process_cell(list[2], env), env);

  // Then update that cell directly
  target_assignment_cell->update_from(*target_assignment_value, env);

  return target_assignment_cell;
}

cell_ptr builtin_fn_env_drop(interpreter_c &ci, cell_list_t &list, env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::DROP, >=, 2)

  for (auto it = std::next(list.begin()); it != list.end(); ++it) {
    if (!env.drop((*it)->as_symbol())) {
      throw interpreter_c::exception_c("Could not find symbol with name :" +
                                           (*it)->as_symbol(),
                                       (*it)->locator);
    }
  }
  return allocate_cell((int64_t)0);
}

cell_ptr builtin_fn_env_fn(interpreter_c &ci, cell_list_t &list, env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::FN, ==, 4)

  auto it = list.begin();

  std::advance(it, 1);

  auto target_function_name = (*it)->as_symbol();

  std::advance(it, 1);

  auto &function_argument_list = (*it)->as_list_info();

  if (function_argument_list.type != list_types_e::DATA) {
    throw interpreter_c::exception_c(
        "Expected data list `[]` for function arguments", (*it)->locator);
  }

  lambda_info_s lambda_info;

  lambda_info.arg_names.reserve(function_argument_list.list.size());

  /*
      Here we COULD check to ensure all parameter names are unique,
      but that would take MORE time than letting stupid choices
      lead to stupid ourcomes
  */

  for (auto &&arg : function_argument_list.list) {
    lambda_info.arg_names.push_back(arg->as_symbol());
  }

  std::advance(it, 1);
  lambda_info.body = (*it);
  if (lambda_info.body->type != cell_type_e::LIST) {
    throw interpreter_c::exception_c("Expected list for function body",
                                     lambda_info.body->locator);
  }

  function_info_s function_info(target_function_name, execute_suspected_lambda,
                                function_type_e::LAMBDA_FUNCTION, &env);

  function_info.lambda = {lambda_info};

  auto fn_cell = allocate_cell(function_info);

  // Set the variable
  env.set(target_function_name, fn_cell);

  return fn_cell;
}

} // namespace builtins
} // namespace nibi
