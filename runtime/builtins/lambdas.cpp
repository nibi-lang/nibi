#include "builtins.hpp"
#include "runtime/cell.hpp"
#include "runtime/environment.hpp"

#include "cpp_macros.hpp"

namespace builtins {

// --------------------------------------------------------
//  Lambda Execution taking the form of builtin functions
// --------------------------------------------------------

cell_c *execute_suspected_lambda(cell_list_t &list, env_c &env) {

  auto it = list.begin();

  auto target_symbol = (*it)->as_symbol();

  auto *target_cell = env.get(target_symbol);

  if (!target_cell) {
    throw runtime_c::exception_c("Symbol not found in environment: " +
                                     (*it)->as_symbol(),
                                 (*it)->locator);
    return nullptr;
  }

  auto &fn_info = target_cell->as_function_info();

  if (fn_info.type != function_type_e::LAMBDA_FUNCTION) {
    throw runtime_c::exception_c("Expected lambda function", (*it)->locator);
  }

  auto &lambda_info = *fn_info.lambda;

  LIST_ENFORCE_SIZE((*it)->as_symbol(), ==, lambda_info.arg_names.size() + 1);

  // Create an environment for the lambda
  // and populate it with the arguments
  auto lambda_env = env_c(env);
  auto &map = lambda_env.get_map();

  for (auto &&arg_name : lambda_info.arg_names) {
    std::advance(it, 1);
    map[arg_name] = global_runtime->execute_cell((*it), env);
  }

  auto &body = lambda_info.body->as_list_info();

  cell_c *result =
      global_runtime->execute_cell(lambda_info.body, lambda_env, true);

  // Because we have pointers to parametrs stored we don't want the environment
  // to free them, so we manually remove them here before

  for (auto &&arg_name : lambda_info.arg_names) {
    map.erase(arg_name);
  }

  // Return a copy of the result
  return result->clone();
}

} // namespace builtins