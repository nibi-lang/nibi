#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "interpreter/interpreter.hpp"
#include "libnibi/cell.hpp"

#include "cpp_macros.hpp"

#include <iterator>

namespace builtins {

cell_ptr builtin_fn_env_assignment(cell_list_t &list, env_c &env) {

  LIST_ENFORCE_SIZE(":=", ==, 3)

  auto it = list.begin();
  std::advance(it, 1);

  if ((*it)->type != cell_type_e::SYMBOL) {
    throw interpreter_c::exception_c("Expected symbol as first argument to :=",
                                     (*it)->locator);
  }

  auto &target_variable_name = (*it)->as_string();

  // Ensure that the user doesn't attempt to overwrite a temporary variable

  if (target_variable_name.starts_with('$')) {
    throw interpreter_c::exception_c(
        "Cannot assign to a variable that starts with a '$' as those are "
        "reserved for temporary variables",
        (*it)->locator);
  }

  auto target_assignment_value =
      global_interpreter->execute_cell(list_get_nth_arg(2, list, env), env);

  // Explicitly clone the value as we might be reading from
  // an instruction that will be mutated later

  target_assignment_value = target_assignment_value->clone();

  env.set(target_variable_name, target_assignment_value);

  // Return a pointer to the new cell so assignments can be chained
  return target_assignment_value;
}

cell_ptr builtin_fn_env_set(cell_list_t &list, env_c &env) {

  LIST_ENFORCE_SIZE("set", ==, 3)

  auto target_assignment_cell =
      global_interpreter->execute_cell(list_get_nth_arg(1, list, env), env);

  auto target_assignment_value =
      global_interpreter->execute_cell(list_get_nth_arg(2, list, env), env);

  // Then update that cell directly
  target_assignment_cell->update_data_and_type_to(*target_assignment_value);

  return target_assignment_cell;
}

cell_ptr builtin_fn_env_drop(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("drop", >=, 2)
  LIST_ITER_SKIP_N(1, {
    if (!env.drop((*it)->as_symbol())) {
      throw interpreter_c::exception_c("Could not find symbol with name :" +
                                           (*it)->as_symbol(),
                                       (*it)->locator);
    }
  })
  return ALLOCATE_CELL((int64_t)0);
}

cell_ptr builtin_fn_env_fn(cell_list_t &list, env_c &env) {

  LIST_ENFORCE_SIZE("fn", ==, 4)

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
                                function_type_e::LAMBDA_FUNCTION);

  function_info.lambda = {lambda_info};

  auto fn_cell = ALLOCATE_CELL(function_info);

  // Set the variable
  env.set(target_function_name, fn_cell);

  return ALLOCATE_CELL((int64_t)0);
}

} // namespace builtins
