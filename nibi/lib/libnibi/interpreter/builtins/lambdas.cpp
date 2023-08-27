#include "libnibi/cell.hpp"
#include "libnibi/environment.hpp"
#include "libnibi/keywords.hpp"

#include "macros.hpp"

#include <iostream>

namespace nibi {
namespace builtins {

// --------------------------------------------------------
//  Lambda Execution taking the form of builtin functions
// --------------------------------------------------------

cell_ptr execute_suspected_lambda(interpreter_c &ci, cell_list_t &list,
                                  env_c &env) {

  auto it = list.begin();

  cell_ptr target_cell = (*it);

  // If the first argument is a symbol, then we need to look it up
  if ((*it)->type == cell_type_e::SYMBOL) {
    auto &target_symbol = (*it)->as_symbol_ref();
    target_cell = env.get(target_symbol);
    if (!target_cell) {
      throw interpreter_c::exception_c("Symbol not found in environment: " +
                                           (*it)->as_symbol(),
                                       (*it)->locator);
      return nullptr;
    }
  }

  auto &fn_info = target_cell->as_function_info();

  if (fn_info.type != function_type_e::LAMBDA_FUNCTION &&
      fn_info.type != function_type_e::FAUX) {
    throw interpreter_c::exception_c("Expected lambda function",
                                     (*it)->locator);
  }

  auto &lambda_info = *fn_info.lambda;

  // Create an environment for the lambda
  // and populate it with the arguments
  auto lambda_env = env_c(fn_info.operating_env);

  auto &map = lambda_env.get_map();

  auto subproc = ci.get_subprocessor(lambda_env);

  if (lambda_info.arg_names.size() == 1 &&
      lambda_info.arg_names[0] == ":args") {

    list_info_s args = {list_types_e::DATA, {}};

    while (it != list.end() - 1) {
      std::advance(it, 1);

      if (fn_info.isolate) {
        args.list.push_back(ci.process_cell((*it), env)->clone(env));
        subproc.set_function_isolation_flag(true);
      } else {
        args.list.push_back(ci.process_cell((*it), env));
      }
    }

    map["$args"] = allocate_cell(args);
    map["$args"]->locator = lambda_info.body->locator;

    // We have a variadic function
  } else {
    NIBI_LIST_ENFORCE_SIZE(nibi::kw::FN, ==, lambda_info.arg_names.size() + 1);

    for (auto &&arg_name : lambda_info.arg_names) {
      std::advance(it, 1);
      NIBI_VALIDATE_VAR_NAME(arg_name, (*it)->locator);
      map[arg_name] = ci.process_cell((*it), env);
      if (fn_info.isolate) {
        map[arg_name] = ci.process_cell((*it), env)->clone(env);
        subproc.set_function_isolation_flag(true);
      } else {
        map[arg_name] = ci.process_cell((*it), env);
      }
    }
  }

  auto &body = lambda_info.body->as_list_info();

  subproc.push_ctx();

  cell_ptr result{nullptr};
  if (subproc.get_function_isolation_flag()) {
    result = subproc.process_cell(lambda_info.body->clone(env, false),
                                  lambda_env, true);
  } else {
    result = subproc.process_cell(lambda_info.body, lambda_env, true);
  }

  subproc.pop_ctx(lambda_env);

  // Because we have pointers to parametrs stored we don't want the environment
  // to free them, so we manually remove them here before

  for (auto &&arg_name : lambda_info.arg_names) {
    map.erase(arg_name);
  }

  // Return a copy of the result
  return result;
}

} // namespace builtins
} // namespace nibi
