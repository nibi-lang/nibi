#pragma once

#include <string>
#include <unordered_map>

#include "runtime/env.hpp"
#include "runtime/cell.hpp"

namespace builtins {

//! \brief Retrieve a reference to a map that ties symbols to their
//!        corresponding builtin function.
std::unordered_map<std::string, function_info_s>& get_builtin_symbols_map();

// Environment modification functions

extern cell_c* builtin_fn_env_assignment(cell_list_t args, env_c &env);
extern cell_c* builtin_fn_env_drop(cell_list_t args, env_c &env);

// Checking functions

extern cell_c* builtin_fn_check_nil(cell_list_t args, env_c &env);
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

extern cell_c* builtin_fn_assert_true(cell_list_t args, env_c &env);
// assert exists (exists!)
// assert is nil (nil!)
// assert is int (int!)
// assert is real (real!)
// assert is string (string!)
// assert is list (list!)
// assert is function (fn!)

// Arithmetic functions

extern cell_c* builtin_fn_arithmetic_add(cell_list_t args, env_c &env);
extern cell_c* builtin_fn_arithmetic_sub(cell_list_t args, env_c &env);
extern cell_c* builtin_fn_arithmetic_div(cell_list_t args, env_c &env);
extern cell_c* builtin_fn_arithmetic_mul(cell_list_t args, env_c &env);
extern cell_c* builtin_fn_arithmetic_mod(cell_list_t args, env_c &env);
extern cell_c* builtin_fn_arithmetic_pow(cell_list_t args, env_c &env);

}