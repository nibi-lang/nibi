#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "libnibi/cell.hpp"

namespace nibi {
namespace builtins {

// All functions in cells are tagged wit their type, their symbol, and contain
// a pointer to their functionality. Here we declare them for all builtin
// functions so all cells point to the same instantiation of the function info
// struct

// arithmetic
static function_info_s builtin_add_inf = {
    "+", builtin_fn_arithmetic_add, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_sub_inf = {
    "-", builtin_fn_arithmetic_sub, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_div_inf = {
    "/", builtin_fn_arithmetic_div, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_mul_inf = {
    "*", builtin_fn_arithmetic_mul, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_mod_inf = {
    "%", builtin_fn_arithmetic_mod, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_pow_inf = {
    "**", builtin_fn_arithmetic_pow, function_type_e::BUILTIN_CPP_FUNCTION};

// bitwise
static function_info_s builtin_bitwise_lsh_inf = {
    "bw-lsh", builtin_fn_bitwise_lsh, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_bitwise_rsh_inf = {
    "bw-rsh", builtin_fn_bitwise_rsh, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_bitwise_and_inf = {
    "bw-and", builtin_fn_bitwise_and, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_bitwise_or_inf = {
    "bw-or", builtin_fn_bitwise_or, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_bitwise_xor_inf = {
    "bw-xor", builtin_fn_bitwise_xor, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_bitwise_not_inf = {
    "bw-not", builtin_fn_bitwise_not, function_type_e::BUILTIN_CPP_FUNCTION};

// environment
static function_info_s builtin_assignment_inf = {
    ":=", builtin_fn_env_assignment, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_set_inf = {
    "set", builtin_fn_env_set, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_drop_inf = {
    "drop", builtin_fn_env_drop, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_fn_inf = {"fn", builtin_fn_env_fn,
                                         function_type_e::BUILTIN_CPP_FUNCTION};

// exceptions
static function_info_s builtin_try_inf = {
    "try", builtin_fn_except_try, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_throw_inf = {
    "throw", builtin_fn_except_throw, function_type_e::BUILTIN_CPP_FUNCTION};

// asserts
static function_info_s builtin_assert_inf = {
    "assert", builtin_fn_assert_true, function_type_e::BUILTIN_CPP_FUNCTION};

// comparison
static function_info_s builtin_comparison_eq_inf = {
    "eq", builtin_fn_comparison_eq, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_comparison_neq_inf = {
    "neq", builtin_fn_comparison_neq, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_comparison_lt_inf = {
    "<", builtin_fn_comparison_lt, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_comparison_gt_inf = {
    ">", builtin_fn_comparison_gt, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_comparison_lte_inf = {
    "<=", builtin_fn_comparison_lte, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_comparison_gte_inf = {
    ">=", builtin_fn_comparison_gte, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_comparison_and_inf = {
    "and", builtin_fn_comparison_and, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_comparison_or_inf = {
    "or", builtin_fn_comparison_or, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_comparison_not_inf = {
    "not", builtin_fn_comparison_not, function_type_e::BUILTIN_CPP_FUNCTION};

// lists
static function_info_s builtin_list_push_front_inf = {
    ">|", builtin_fn_list_push_front, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_list_push_back_inf = {
    "|<", builtin_fn_list_push_back, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_list_spawn_inf = {
    "<|>", builtin_fn_list_spawn, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_list_iter_inf = {
    "iter", builtin_fn_list_iter, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_list_at_inf = {
    "at", builtin_fn_list_at, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_list_pop_front_inf = {
    "<<|", builtin_fn_list_pop_front, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_list_pop_back_inf = {
    "|>>", builtin_fn_list_pop_back, function_type_e::BUILTIN_CPP_FUNCTION};

// common functions
static function_info_s builtin_common_len_inf = {
    "len", builtin_fn_common_len, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_yield_inf = {
    "<-", builtin_fn_common_yield, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_loop_inf = {
    "loop", builtin_fn_common_loop, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_if_inf = {
    "?", builtin_fn_common_if, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_clone_inf = {
    "clone", builtin_fn_common_clone, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_put_inf = {
    "put", builtin_fn_common_put, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_putln_inf = {
    "putln", builtin_fn_common_putln, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_import_inf = {
    "import", builtin_fn_common_import, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_use_inf = {
    "use", builtin_fn_common_use, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_exit_inf = {
    "exit", builtin_fn_common_exit, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_eval_inf = {
    "eval", builtin_fn_common_eval, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_quote_inf = {
    "quote", builtin_fn_common_quote, function_type_e::BUILTIN_CPP_FUNCTION};

// environment cell functions
static function_info_s builtin_envcell_env_inf = {
    "env", builtin_fn_envcell_env, function_type_e::BUILTIN_CPP_FUNCTION};

// conversion functions

static function_info_s builtin_cvt_string_inf = {
    "str", builtin_fn_cvt_to_string, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_cvt_int_inf = {
    "int", builtin_fn_cvt_to_integer, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_cvt_float_inf = {
    "float", builtin_fn_cvt_to_float, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_cvt_split_inf = {
    "split", builtin_fn_cvt_to_split, function_type_e::BUILTIN_CPP_FUNCTION};

// This map is used to look up the function info struct for a given symbol
static phmap::parallel_node_hash_map<std::string, function_info_s> keyword_map =
    {{"eq", builtin_comparison_eq_inf},
     {"neq", builtin_comparison_neq_inf},
     {"<", builtin_comparison_lt_inf},
     {">", builtin_comparison_gt_inf},
     {"<=", builtin_comparison_lte_inf},
     {">=", builtin_comparison_gte_inf},
     {"and", builtin_comparison_and_inf},
     {"or", builtin_comparison_or_inf},
     {"not", builtin_comparison_not_inf},
     {"+", builtin_add_inf},
     {"-", builtin_sub_inf},
     {"/", builtin_div_inf},
     {"*", builtin_mul_inf},
     {"%", builtin_mod_inf},
     {"**", builtin_pow_inf},
     {":=", builtin_assignment_inf},
     {"set", builtin_set_inf},
     {"fn", builtin_fn_inf},
     {"drop", builtin_drop_inf},
     {"try", builtin_try_inf},
     {"throw", builtin_throw_inf},
     {"assert", builtin_assert_inf},
     {">|", builtin_list_push_front_inf},
     {"|<", builtin_list_push_back_inf},
     {"<<|", builtin_list_pop_front_inf},
     {"|>>", builtin_list_pop_back_inf},
     {"<|>", builtin_list_spawn_inf},
     {"iter", builtin_list_iter_inf},
     {"at", builtin_list_at_inf},
     {"len", builtin_common_len_inf},
     {"<-", builtin_common_yield_inf},
     {"loop", builtin_common_loop_inf},
     {"?", builtin_common_if_inf},
     {"clone", builtin_common_clone_inf},
     {"put", builtin_common_put_inf},
     {"putln", builtin_common_putln_inf},
     {"import", builtin_common_import_inf},
     {"use", builtin_common_use_inf},
     {"exit", builtin_common_exit_inf},
     {"eval", builtin_common_eval_inf},
     {"quote", builtin_common_quote_inf},
     {"bw-lsh", builtin_bitwise_lsh_inf},
     {"bw-rsh", builtin_bitwise_rsh_inf},
     {"bw-and", builtin_bitwise_and_inf},
     {"bw-or", builtin_bitwise_or_inf},
     {"bw-xor", builtin_bitwise_xor_inf},
     {"bw-not", builtin_bitwise_not_inf},
     {"env", builtin_envcell_env_inf},
     {"str", builtin_cvt_string_inf},
     {"int", builtin_cvt_int_inf},
     {"float", builtin_cvt_float_inf},
     {"split", builtin_cvt_split_inf}};

// Retrieve the map of symbols to function info structs
function_router_t get_builtin_symbols_map() { return keyword_map; }

} // namespace builtins
} // namespace nibi
