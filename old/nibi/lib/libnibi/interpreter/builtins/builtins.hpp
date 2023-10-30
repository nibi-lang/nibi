#pragma once

#include <string>
#include <unordered_map>

#include "libnibi/cell.hpp"
#include "libnibi/environment.hpp"
#include "libnibi/types.hpp"

namespace nibi {

class interpreter_c;

namespace builtins {

//! \brief Retrieve a reference to a map that ties symbols to their
//!        corresponding builtin function.
function_router_t &get_builtin_symbols_map();

//! \brief A function similar to the builtins that
//!        will load a lambda function and execute it
//!        using the global runtime object
//! \param list The list containing the lambda function and args
//! \param env The environment that will be used during execution
extern cell_ptr execute_suspected_lambda(interpreter_c &ci, cell_list_t &list,
                                         env_c &env);

// Environment modification functions

extern cell_ptr builtin_fn_env_alias(interpreter_c &ci, cell_list_t &list,
                                     env_c &env);
extern cell_ptr builtin_fn_env_assignment(interpreter_c &ci, cell_list_t &list,
                                          env_c &env);
extern cell_ptr builtin_fn_env_set(interpreter_c &ci, cell_list_t &list,
                                   env_c &env);
extern cell_ptr builtin_fn_env_drop(interpreter_c &ci, cell_list_t &list,
                                    env_c &env);
extern cell_ptr builtin_fn_env_fn(interpreter_c &ci, cell_list_t &list,
                                  env_c &env);
extern cell_ptr builtin_fn_env_str_set_at(interpreter_c &ci, cell_list_t &list,
                                          env_c &env);
extern cell_ptr builtin_fn_dict_fn(interpreter_c &ci, cell_list_t &list,
                                   env_c &env);

// Exception throwing and handling functions

extern cell_ptr builtin_fn_except_try(interpreter_c &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_except_throw(interpreter_c &ci, cell_list_t &list,
                                        env_c &env);

// List functions

extern cell_ptr builtin_fn_list_push_front(interpreter_c &ci, cell_list_t &list,
                                           env_c &env);
extern cell_ptr builtin_fn_list_push_back(interpreter_c &ci, cell_list_t &list,
                                          env_c &env);
extern cell_ptr builtin_fn_list_iter(interpreter_c &ci, cell_list_t &list,
                                     env_c &env);
extern cell_ptr builtin_fn_list_at(interpreter_c &ci, cell_list_t &list,
                                   env_c &env);
extern cell_ptr builtin_fn_list_spawn(interpreter_c &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_list_pop_front(interpreter_c &ci, cell_list_t &list,
                                          env_c &env);
extern cell_ptr builtin_fn_list_pop_back(interpreter_c &ci, cell_list_t &list,
                                         env_c &env);

// Common functions

extern cell_ptr builtin_fn_common_clone(interpreter_c &ci, cell_list_t &list,
                                        env_c &env);
extern cell_ptr builtin_fn_common_len(interpreter_c &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_common_yield(interpreter_c &ci, cell_list_t &list,
                                        env_c &env);
extern cell_ptr builtin_fn_common_loop(interpreter_c &ci, cell_list_t &list,
                                       env_c &env);
extern cell_ptr builtin_fn_common_if(interpreter_c &ci, cell_list_t &list,
                                     env_c &env);
extern cell_ptr builtin_fn_common_import(interpreter_c &ci, cell_list_t &list,
                                         env_c &env);
extern cell_ptr builtin_fn_common_use(interpreter_c &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_common_exit(interpreter_c &ci, cell_list_t &list,
                                       env_c &env);
extern cell_ptr builtin_fn_common_quote(interpreter_c &ci, cell_list_t &list,
                                        env_c &env);
extern cell_ptr builtin_fn_common_eval(interpreter_c &ci, cell_list_t &list,
                                       env_c &env);
extern cell_ptr builtin_fn_common_nop(interpreter_c &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_common_macro(interpreter_c &ci, cell_list_t &list,
                                        env_c &env);
extern cell_ptr builtin_fn_common_exchange(interpreter_c &ci, cell_list_t &list,
                                           env_c &env);
extern cell_ptr builtin_fn_common_defer(interpreter_c &ci, cell_list_t &list,
                                        env_c &env);

// Assertions

extern cell_ptr builtin_fn_assert_true(interpreter_c &ci, cell_list_t &list,
                                       env_c &env);

// Arithmetic functions

extern cell_ptr builtin_fn_arithmetic_add(interpreter_c &ci, cell_list_t &list,
                                          env_c &env);
extern cell_ptr builtin_fn_arithmetic_sub(interpreter_c &ci, cell_list_t &list,
                                          env_c &env);
extern cell_ptr builtin_fn_arithmetic_div(interpreter_c &ci, cell_list_t &list,
                                          env_c &env);
extern cell_ptr builtin_fn_arithmetic_mul(interpreter_c &ci, cell_list_t &list,
                                          env_c &env);
extern cell_ptr builtin_fn_arithmetic_mod(interpreter_c &ci, cell_list_t &list,
                                          env_c &env);
extern cell_ptr builtin_fn_arithmetic_pow(interpreter_c &ci, cell_list_t &list,
                                          env_c &env);

// Bitwise functions

extern cell_ptr builtin_fn_bitwise_lsh(interpreter_c &ci, cell_list_t &list,
                                       env_c &env);
extern cell_ptr builtin_fn_bitwise_rsh(interpreter_c &ci, cell_list_t &list,
                                       env_c &env);
extern cell_ptr builtin_fn_bitwise_and(interpreter_c &ci, cell_list_t &list,
                                       env_c &env);
extern cell_ptr builtin_fn_bitwise_or(interpreter_c &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_bitwise_xor(interpreter_c &ci, cell_list_t &list,
                                       env_c &env);
extern cell_ptr builtin_fn_bitwise_not(interpreter_c &ci, cell_list_t &list,
                                       env_c &env);

// Comparison functions

extern cell_ptr builtin_fn_comparison_eq(interpreter_c &ci, cell_list_t &list,
                                         env_c &env);
extern cell_ptr builtin_fn_comparison_neq(interpreter_c &ci, cell_list_t &list,
                                          env_c &env);
extern cell_ptr builtin_fn_comparison_lt(interpreter_c &ci, cell_list_t &list,
                                         env_c &env);
extern cell_ptr builtin_fn_comparison_gt(interpreter_c &ci, cell_list_t &list,
                                         env_c &env);
extern cell_ptr builtin_fn_comparison_lte(interpreter_c &ci, cell_list_t &list,
                                          env_c &env);
extern cell_ptr builtin_fn_comparison_gte(interpreter_c &ci, cell_list_t &list,
                                          env_c &env);
extern cell_ptr builtin_fn_comparison_and(interpreter_c &ci, cell_list_t &list,
                                          env_c &env);
extern cell_ptr builtin_fn_comparison_or(interpreter_c &ci, cell_list_t &list,
                                         env_c &env);
extern cell_ptr builtin_fn_comparison_not(interpreter_c &ci, cell_list_t &list,
                                          env_c &env);

// Conversion functions

extern cell_ptr builtin_fn_cvt_to_string(interpreter_c &ci, cell_list_t &list,
                                         env_c &env);
extern cell_ptr builtin_fn_cvt_to_string_lit(interpreter_c &ci,
                                             cell_list_t &list, env_c &env);
extern cell_ptr builtin_fn_cvt_to_integer(interpreter_c &ci, cell_list_t &list,
                                          env_c &env);
extern cell_ptr builtin_fn_cvt_to_float(interpreter_c &ci, cell_list_t &list,
                                        env_c &env);
extern cell_ptr builtin_fn_cvt_to_split(interpreter_c &ci, cell_list_t &list,
                                        env_c &env);

extern cell_ptr builtin_fn_cvt_to_i8(interpreter_c &ci, cell_list_t &list,
                                     env_c &env);
extern cell_ptr builtin_fn_cvt_to_i16(interpreter_c &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_cvt_to_i32(interpreter_c &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_cvt_to_i64(interpreter_c &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_cvt_to_u8(interpreter_c &ci, cell_list_t &list,
                                     env_c &env);
extern cell_ptr builtin_fn_cvt_to_u16(interpreter_c &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_cvt_to_u32(interpreter_c &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_cvt_to_u64(interpreter_c &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_cvt_to_f32(interpreter_c &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_cvt_to_f64(interpreter_c &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_cvt_to_char(interpreter_c &ci, cell_list_t &list,
                                       env_c &env);

// Reflection

extern cell_ptr builtin_fn_reflect_type(interpreter_c &ci, cell_list_t &list,
                                        env_c &env);

// External calling

extern cell_ptr builtin_fn_extern_call(interpreter_c &ci, cell_list_t &list,
                                       env_c &env);
// Manual memory ops

extern cell_ptr builtin_fn_memory_new(interpreter_c &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_memory_del(interpreter_c &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_memory_cpy(interpreter_c &ci, cell_list_t &list,
                                      env_c &env);
extern cell_ptr builtin_fn_memory_load(interpreter_c &ci, cell_list_t &list,
                                       env_c &env);
extern cell_ptr builtin_fn_memory_is_set(interpreter_c &ci, cell_list_t &list,
                                         env_c &env);

} // namespace builtins
} // namespace nibi
