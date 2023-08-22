#include "lib.hpp"
#include "cpp-net-lib/netlib.hpp"
#include <chrono>
#include <libnibi/interpreter/builtins/builtins.hpp>
#include <libnibi/macros.hpp>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

nibi::cell_ptr nibi_threads_sleep(nibi::interpreter_c &ci,
                                  nibi::cell_list_t &list, nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{threads nibi_threads_sleep}", ==, 2)
  auto time = ci.process_cell(list[1], env)->as_integer();
  std::this_thread::sleep_for(std::chrono::microseconds(time));
  return nibi::allocate_cell((uint64_t)time);
}

nibi::cell_ptr nibi_threads_fn(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env) {

  NIBI_LIST_ENFORCE_SIZE("{threads nibi_threads_fn}", ==, 4)

  auto it = list.begin();

  std::advance(it, 1);

  auto target_function_name = (*it)->as_symbol();

  std::advance(it, 1);

  auto &function_argument_list = (*it)->as_list_info();

  if (function_argument_list.type != nibi::list_types_e::DATA) {
    throw nibi::interpreter_c::exception_c(
        "Expected data list `[]` for thread function arguments",
        (*it)->locator);
  }

  nibi::lambda_info_s lambda_info;

  lambda_info.arg_names.reserve(function_argument_list.list.size());

  for (auto &&arg : function_argument_list.list) {
    lambda_info.arg_names.push_back(arg->as_symbol());
  }

  std::advance(it, 1);
  lambda_info.body = (*it)->clone(env, false);
  if (lambda_info.body->type != nibi::cell_type_e::LIST) {
    throw nibi::interpreter_c::exception_c("Expected list for function body",
                                           lambda_info.body->locator);
  }

  // We make a faux so we control our own env, but we pipe it through
  // the standard lambda exec method
  nibi::function_info_s function_info(
      target_function_name, nibi::builtins::execute_suspected_lambda,
      nibi::function_type_e::FAUX, new nibi::env_c(env));

  function_info.isolate = true;

  // Set the lambda info
  function_info.lambda = {lambda_info};

  auto fn_cell = nibi::allocate_cell(function_info);
  fn_cell->locator = list[0]->locator;

  // Set the variable
  env.set(target_function_name, fn_cell);

  return std::move(fn_cell);
}
