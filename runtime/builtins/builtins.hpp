#pragma once

#include <string>
#include <unordered_map>

#include "runtime/cell.hpp"
#include "runtime/environment.hpp"

namespace builtins {

//! \brief Retrieve a reference to a map that ties symbols to their
//!        corresponding builtin function.
std::unordered_map<std::string, function_info_s> &get_builtin_symbols_map();

// @ commands

extern cell_c *builtin_fn_at_debug(cell_list_t &list, env_c &env);

// Debug helper functions

extern cell_c *builtin_fn_debug_dbg_dbg(cell_list_t &list, env_c &env);
extern cell_c *builtin_fn_debug_dbg_var(cell_list_t &list, env_c &env);
extern cell_c *builtin_fn_debug_dbg_out(cell_list_t &list, env_c &env);

// Environment modification functions

extern cell_c *builtin_fn_env_assignment(cell_list_t &list, env_c &env);
extern cell_c *builtin_fn_env_set(cell_list_t &list, env_c &env);
extern cell_c *builtin_fn_env_drop(cell_list_t &list, env_c &env);

// Exception throwing and handling functions

extern cell_c *builtin_fn_except_try(cell_list_t &list, env_c &env);
extern cell_c *builtin_fn_except_throw(cell_list_t &list, env_c &env);

// Checking functions

extern cell_c *builtin_fn_check_nil(cell_list_t &list, env_c &env);
// check is true (true?)
// check is true (false?)
// check exists (exists?)
// check is nil (nil?)
// check is int (int?)
// check is real (real?)
// check is string (string?)
// check is list (list?)
// check is function (fn?)

// Assertions

extern cell_c *builtin_fn_assert_true(cell_list_t &list, env_c &env);

// Arithmetic functions

extern cell_c *builtin_fn_arithmetic_add(cell_list_t &list, env_c &env);
extern cell_c *builtin_fn_arithmetic_sub(cell_list_t &list, env_c &env);
extern cell_c *builtin_fn_arithmetic_div(cell_list_t &list, env_c &env);
extern cell_c *builtin_fn_arithmetic_mul(cell_list_t &list, env_c &env);
extern cell_c *builtin_fn_arithmetic_mod(cell_list_t &list, env_c &env);
extern cell_c *builtin_fn_arithmetic_pow(cell_list_t &list, env_c &env);

// Comparison functions

extern cell_c *builtin_fn_comparison_eq(cell_list_t &list, env_c &env);
extern cell_c *builtin_fn_comparison_neq(cell_list_t &list, env_c &env);
extern cell_c *builtin_fn_comparison_lt(cell_list_t &list, env_c &env);
extern cell_c *builtin_fn_comparison_gt(cell_list_t &list, env_c &env);
extern cell_c *builtin_fn_comparison_lte(cell_list_t &list, env_c &env);
extern cell_c *builtin_fn_comparison_gte(cell_list_t &list, env_c &env);

} // namespace builtins