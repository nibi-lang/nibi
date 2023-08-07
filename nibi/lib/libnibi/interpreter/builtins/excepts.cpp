#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "interpreter/interpreter.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/keywords.hpp"
#include "macros.hpp"

#include <iterator>

namespace nibi {
namespace builtins {

namespace {
cell_ptr handle_thrown_error_in_try(std::string message, cell_ptr recover_cell,
                                    cell_processor_if &ci, env_c &env) {
  auto e_cell = allocate_cell(message);
  env.set(nibi::kw::TERR, e_cell);
  auto result = ci.process_cell(recover_cell, env, true);
  env.drop(nibi::kw::TERR);
  return result;
}
} // namespace

cell_ptr builtin_fn_except_try(cell_processor_if &ci, cell_list_t &list,
                               env_c &env) {

  /*
    (try () ())

    The first param can be a process list, or a data list that will be walked
    and itered

    The second param is a process list of a data list used to catch the error
    and deal with it

    Temporarily, we will inject an environment with `$e` set to the error
    message

  */

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::TRY, ==, 3)

  auto it = list.begin();

  std::advance(it, 1);
  auto attempt_cell = (*it);

  std::advance(it, 1);
  auto recover_cell = (*it);

  env_c try_env(&env);
  ci.push_ctx();

  cell_ptr res{nullptr};

  try {
    // Call execute with the process_data_cell flag set to true
    // which will allow us to walk over multiple cells and catch on them
    res = ci.process_cell(attempt_cell, env, true);
  } catch (interpreter_c::exception_c &e) {
    res = handle_thrown_error_in_try(e.what(), recover_cell, ci, try_env);
  } catch (cell_access_exception_c &e) {
    res = handle_thrown_error_in_try(e.what(), recover_cell, ci, try_env);
  }

  ci.pop_ctx(try_env);
  return res;
}

cell_ptr builtin_fn_except_throw(cell_processor_if &ci, cell_list_t &list,
                                 env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::THROW, ==, 2)

  auto it = list.begin();

  std::advance(it, 1);
  auto exec_cell = (*it);

  auto thrown = ci.process_cell(exec_cell, env, true);

  throw interpreter_c::exception_c(thrown->to_string(), list.front()->locator);
}

} // namespace builtins
} // namespace nibi
