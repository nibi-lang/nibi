#pragma once

#include <string>
#include <unordered_map>

#include "libnibi/cell.hpp"
#include "libnibi/environment.hpp"
#include "libnibi/interfaces/cell_processor_if.hpp"
#include "libnibi/parallel_hashmap/phmap.hpp"
#include "libnibi/types.hpp"

namespace nibi {
namespace builtins {

//! \brief Retrieve a reference to a map that ties symbols to their
//!        corresponding builtin function.
function_router_t &get_builtin_symbols_map();

//! \brief A function similar to the builtins that
//!        will load a lambda function and execute it
//!        using the global runtime object
//! \param list The list containing the lambda function and args
//! \param env The environment that will be used during execution
extern cell_ptr execute_suspected_lambda(cell_processor_if &ci,
                                         cell_list_t &list, env_c &env);

// Environment modification functions

extern cell_ptr builtin_fn_env_assignment(cell_processor_if &ci,
                                          cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_env_set(cell_processor_if &ci, cell_list_t &list,
                                   env_c &env);
extern cell_ptr builtin_fn_env_drop(cell_processor_if &ci, cell_list_t &list,
                                    env_c &env);
extern cell_ptr builtin_fn_env_fn(cell_processor_if &ci, cell_list_t &list,
                                  env_c &env);

// Exception throwing and handling functions

extern cell_ptr builtin_fn_except_try(cell_processor_if &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_except_throw(cell_processor_if &ci,
                                        cell_list_t &list, env_c &env);

// List functions

extern cell_ptr builtin_fn_list_push_front(cell_processor_if &ci,
                                           cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_list_push_back(cell_processor_if &ci,
                                          cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_list_iter(cell_processor_if &ci, cell_list_t &list,
                                     env_c &env);
extern cell_ptr builtin_fn_list_at(cell_processor_if &ci, cell_list_t &list,
                                   env_c &env);
extern cell_ptr builtin_fn_list_spawn(cell_processor_if &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_list_pop_front(cell_processor_if &ci,
                                          cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_list_pop_back(cell_processor_if &ci,
                                         cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_list_execute(cell_processor_if &ci,
                                         cell_list_t &list, env_c &env);

// Common functions

extern cell_ptr builtin_fn_common_clone(cell_processor_if &ci,
                                        cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_common_len(cell_processor_if &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_common_yield(cell_processor_if &ci,
                                        cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_common_loop(cell_processor_if &ci, cell_list_t &list,
                                       env_c &env);
extern cell_ptr builtin_fn_common_if(cell_processor_if &ci, cell_list_t &list,
                                     env_c &env);
extern cell_ptr builtin_fn_common_import(cell_processor_if &ci,
                                         cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_common_use(cell_processor_if &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_common_exit(cell_processor_if &ci, cell_list_t &list,
                                       env_c &env);
extern cell_ptr builtin_fn_common_quote(cell_processor_if &ci,
                                        cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_common_eval(cell_processor_if &ci, cell_list_t &list,
                                       env_c &env);
extern cell_ptr builtin_fn_common_nop(cell_processor_if &ci, cell_list_t &list,
                                      env_c &env);

// Assertions

extern cell_ptr builtin_fn_assert_true(cell_processor_if &ci, cell_list_t &list,
                                       env_c &env);
extern cell_ptr builtin_fn_assert_eq(cell_processor_if &ci, cell_list_t &list,
                                     env_c &env);
extern cell_ptr builtin_fn_assert_neq(cell_processor_if &ci, cell_list_t &list,
                                      env_c &env);

// Arithmetic functions

extern cell_ptr builtin_fn_arithmetic_add(cell_processor_if &ci,
                                          cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_arithmetic_sub(cell_processor_if &ci,
                                          cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_arithmetic_div(cell_processor_if &ci,
                                          cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_arithmetic_mul(cell_processor_if &ci,
                                          cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_arithmetic_mod(cell_processor_if &ci,
                                          cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_arithmetic_pow(cell_processor_if &ci,
                                          cell_list_t &list, env_c &env);

// Bitwise functions

extern cell_ptr builtin_fn_bitwise_lsh(cell_processor_if &ci, cell_list_t &list,
                                       env_c &env);
extern cell_ptr builtin_fn_bitwise_rsh(cell_processor_if &ci, cell_list_t &list,
                                       env_c &env);
extern cell_ptr builtin_fn_bitwise_and(cell_processor_if &ci, cell_list_t &list,
                                       env_c &env);
extern cell_ptr builtin_fn_bitwise_or(cell_processor_if &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_bitwise_xor(cell_processor_if &ci, cell_list_t &list,
                                       env_c &env);
extern cell_ptr builtin_fn_bitwise_not(cell_processor_if &ci, cell_list_t &list,
                                       env_c &env);

// Comparison functions

extern cell_ptr builtin_fn_comparison_eq(cell_processor_if &ci,
                                         cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_comparison_neq(cell_processor_if &ci,
                                          cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_comparison_lt(cell_processor_if &ci,
                                         cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_comparison_gt(cell_processor_if &ci,
                                         cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_comparison_lte(cell_processor_if &ci,
                                          cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_comparison_gte(cell_processor_if &ci,
                                          cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_comparison_and(cell_processor_if &ci,
                                          cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_comparison_or(cell_processor_if &ci,
                                         cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_comparison_not(cell_processor_if &ci,
                                          cell_list_t &list, env_c &env);

// Conversion functions

extern cell_ptr builtin_fn_cvt_to_string(cell_processor_if &ci,
                                         cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_cvt_to_integer(cell_processor_if &ci,
                                          cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_cvt_to_float(cell_processor_if &ci,
                                        cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_cvt_to_split(cell_processor_if &ci,
                                        cell_list_t &list, env_c &env);

// Reflection

extern cell_ptr builtin_fn_reflect_type(cell_processor_if &ci,
                                        cell_list_t &list, env_c &env);

} // namespace builtins
} // namespace nibi
