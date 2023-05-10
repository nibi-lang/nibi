#pragma once

#include <string>
#include <unordered_map>

#include "libnibi/cell.hpp"
#include "libnibi/environment.hpp"
#include "libnibi/parallel_hashmap/phmap.hpp"

namespace builtins {

//! \brief Retrieve a reference to a map that ties symbols to their
//!        corresponding builtin function.
phmap::parallel_node_hash_map<std::string, function_info_s> &
get_builtin_symbols_map();

//! \brief A function similar to the builtins that
//!        will load a lambda function and execute it
//!        using the global runtime object
//! \param list The list containing the lambda function and args
//! \param env The environment that will be used during execution
extern cell_ptr execute_suspected_lambda(cell_list_t &list, env_c &env);

// @ commands

extern cell_ptr builtin_fn_at_debug(cell_list_t &list, env_c &env);

// Debug helper functions

extern cell_ptr builtin_fn_debug_dbg_dbg(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_debug_dbg_var(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_debug_dbg_out(cell_list_t &list, env_c &env);

// Environment modification functions

extern cell_ptr builtin_fn_env_assignment(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_env_set(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_env_drop(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_env_fn(cell_list_t &list, env_c &env);

// Exception throwing and handling functions

extern cell_ptr builtin_fn_except_try(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_except_throw(cell_list_t &list, env_c &env);

// List functions

extern cell_ptr builtin_fn_list_push_front(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_list_push_back(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_list_iter(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_list_at(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_list_spawn(cell_list_t &list, env_c &env);

// Common functions

extern cell_ptr builtin_fn_common_clone(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_common_len(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_common_yield(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_common_loop(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_common_if(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_common_put(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_common_putln(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_common_import(cell_list_t &list, env_c &env);

// Assertions

extern cell_ptr builtin_fn_assert_true(cell_list_t &list, env_c &env);

// Arithmetic functions

extern cell_ptr builtin_fn_arithmetic_add(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_arithmetic_sub(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_arithmetic_div(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_arithmetic_mul(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_arithmetic_mod(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_arithmetic_pow(cell_list_t &list, env_c &env);

// Bitwise functions

extern cell_ptr builtin_fn_bitwise_lsh(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_bitwise_rsh(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_bitwise_and(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_bitwise_or(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_bitwise_xor(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_bitwise_not(cell_list_t &list, env_c &env);

// Comparison functions

extern cell_ptr builtin_fn_comparison_eq(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_comparison_neq(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_comparison_lt(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_comparison_gt(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_comparison_lte(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_comparison_gte(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_comparison_and(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_comparison_or(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_comparison_not(cell_list_t &list, env_c &env);

// Environment cell functions

extern cell_ptr builtin_fn_envcell_env(cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_envcell_create(cell_list_t &list, env_c &env);

} // namespace builtins