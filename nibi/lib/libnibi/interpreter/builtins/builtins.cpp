#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/keywords.hpp"

namespace nibi {
namespace builtins {

// All functions in cells are tagged wit their type, their symbol, and contain
// a pointer to their functionality. Here we declare them for all builtin
// functions so all cells point to the same instantiation of the function info
// struct

// arithmetic
static function_info_s builtin_add_inf = {
    nibi::kw::ADD, builtin_fn_arithmetic_add,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_sub_inf = {
    nibi::kw::SUB, builtin_fn_arithmetic_sub,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_div_inf = {
    nibi::kw::DIV, builtin_fn_arithmetic_div,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_mul_inf = {
    nibi::kw::MUL, builtin_fn_arithmetic_mul,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_mod_inf = {
    nibi::kw::MOD, builtin_fn_arithmetic_mod,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_pow_inf = {
    nibi::kw::POW, builtin_fn_arithmetic_pow,
    function_type_e::BUILTIN_CPP_FUNCTION};

// bitwise
static function_info_s builtin_bitwise_lsh_inf = {
    nibi::kw::BW_LSH, builtin_fn_bitwise_lsh,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_bitwise_rsh_inf = {
    nibi::kw::BW_RSH, builtin_fn_bitwise_rsh,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_bitwise_and_inf = {
    nibi::kw::BW_AND, builtin_fn_bitwise_and,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_bitwise_or_inf = {
    nibi::kw::BW_OR, builtin_fn_bitwise_or,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_bitwise_xor_inf = {
    nibi::kw::BW_XOR, builtin_fn_bitwise_xor,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_bitwise_not_inf = {
    nibi::kw::BW_NOT, builtin_fn_bitwise_not,
    function_type_e::BUILTIN_CPP_FUNCTION};

// environment
static function_info_s builtin_assignment_inf = {
    nibi::kw::ASSIGN, builtin_fn_env_assignment,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_set_inf = {
    nibi::kw::SET, builtin_fn_env_set, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_drop_inf = {
    nibi::kw::DROP, builtin_fn_env_drop, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_fn_inf = {nibi::kw::FN, builtin_fn_env_fn,
                                         function_type_e::BUILTIN_CPP_FUNCTION};

// exceptions
static function_info_s builtin_try_inf = {
    nibi::kw::TRY, builtin_fn_except_try,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_throw_inf = {
    nibi::kw::THROW, builtin_fn_except_throw,
    function_type_e::BUILTIN_CPP_FUNCTION};

// asserts
static function_info_s builtin_assert_inf = {
    nibi::kw::ASSERT, builtin_fn_assert_true,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_assert_eq_inf = {
    nibi::kw::ASSERT_EQ, builtin_fn_assert_eq,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_assert_neq_inf = {
    nibi::kw::ASSERT_NEQ, builtin_fn_assert_neq,
    function_type_e::BUILTIN_CPP_FUNCTION};

// comparison
static function_info_s builtin_comparison_eq_inf = {
    nibi::kw::EQ, builtin_fn_comparison_eq,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_comparison_neq_inf = {
    nibi::kw::NEQ, builtin_fn_comparison_neq,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_comparison_lt_inf = {
    nibi::kw::LT, builtin_fn_comparison_lt,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_comparison_gt_inf = {
    nibi::kw::GT, builtin_fn_comparison_gt,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_comparison_lte_inf = {
    nibi::kw::LTE, builtin_fn_comparison_lte,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_comparison_gte_inf = {
    nibi::kw::GTE, builtin_fn_comparison_gte,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_comparison_and_inf = {
    nibi::kw::AND, builtin_fn_comparison_and,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_comparison_or_inf = {
    nibi::kw::OR, builtin_fn_comparison_or,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_comparison_not_inf = {
    nibi::kw::NOT, builtin_fn_comparison_not,
    function_type_e::BUILTIN_CPP_FUNCTION};

// lists
static function_info_s builtin_list_push_front_inf = {
    nibi::kw::PUSH_FRONT, builtin_fn_list_push_front,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_list_push_back_inf = {
    nibi::kw::PUSH_BACK, builtin_fn_list_push_back,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_list_spawn_inf = {
    nibi::kw::SPAWN, builtin_fn_list_spawn,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_list_iter_inf = {
    nibi::kw::ITER, builtin_fn_list_iter,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_list_at_inf = {
    nibi::kw::AT, builtin_fn_list_at, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_list_pop_front_inf = {
    nibi::kw::POP_FRONT, builtin_fn_list_pop_front,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_list_pop_back_inf = {
    nibi::kw::POP_BACK, builtin_fn_list_pop_back,
    function_type_e::BUILTIN_CPP_FUNCTION};

// common functions
static function_info_s builtin_common_len_inf = {
    nibi::kw::LEN, builtin_fn_common_len,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_yield_inf = {
    nibi::kw::YIELD, builtin_fn_common_yield,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_loop_inf = {
    nibi::kw::LOOP, builtin_fn_common_loop,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_if_inf = {
    nibi::kw::IF, builtin_fn_common_if, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_clone_inf = {
    nibi::kw::CLONE, builtin_fn_common_clone,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_put_inf = {
    nibi::kw::PUT, builtin_fn_common_put,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_putln_inf = {
    nibi::kw::PUTLN, builtin_fn_common_putln,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_import_inf = {
    nibi::kw::IMPORT, builtin_fn_common_import,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_use_inf = {
    nibi::kw::USE, builtin_fn_common_use,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_exit_inf = {
    nibi::kw::EXIT, builtin_fn_common_exit,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_eval_inf = {
    nibi::kw::EVAL, builtin_fn_common_eval,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_quote_inf = {
    nibi::kw::QUOTE, builtin_fn_common_quote,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_common_nop_inf = {
    nibi::kw::NOP, builtin_fn_common_nop,
    function_type_e::BUILTIN_CPP_FUNCTION};

// conversion functions

static function_info_s builtin_cvt_string_inf = {
    nibi::kw::STR, builtin_fn_cvt_to_string,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_cvt_int_inf = {
    nibi::kw::INT, builtin_fn_cvt_to_integer,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_cvt_float_inf = {
    nibi::kw::FLOAT, builtin_fn_cvt_to_float,
    function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_cvt_split_inf = {
    nibi::kw::SPLIT, builtin_fn_cvt_to_split,
    function_type_e::BUILTIN_CPP_FUNCTION};

// reflection

static function_info_s builtin_reflect_type_inf = {
    nibi::kw::TYPE, builtin_fn_reflect_type,
    function_type_e::BUILTIN_CPP_FUNCTION};

// This map is used to look up the function info struct for a given symbol
static phmap::parallel_node_hash_map<std::string, function_info_s> keyword_map =
    {{nibi::kw::EQ, builtin_comparison_eq_inf},
     {nibi::kw::NEQ, builtin_comparison_neq_inf},
     {nibi::kw::LT, builtin_comparison_lt_inf},
     {nibi::kw::GT, builtin_comparison_gt_inf},
     {nibi::kw::LTE, builtin_comparison_lte_inf},
     {nibi::kw::GTE, builtin_comparison_gte_inf},
     {nibi::kw::AND, builtin_comparison_and_inf},
     {nibi::kw::OR, builtin_comparison_or_inf},
     {nibi::kw::NOT, builtin_comparison_not_inf},
     {nibi::kw::ADD, builtin_add_inf},
     {nibi::kw::SUB, builtin_sub_inf},
     {nibi::kw::DIV, builtin_div_inf},
     {nibi::kw::MUL, builtin_mul_inf},
     {nibi::kw::MOD, builtin_mod_inf},
     {nibi::kw::POW, builtin_pow_inf},
     {nibi::kw::ASSIGN, builtin_assignment_inf},
     {nibi::kw::SET, builtin_set_inf},
     {nibi::kw::FN, builtin_fn_inf},
     {nibi::kw::DROP, builtin_drop_inf},
     {nibi::kw::TRY, builtin_try_inf},
     {nibi::kw::THROW, builtin_throw_inf},
     {nibi::kw::ASSERT, builtin_assert_inf},
     {nibi::kw::ASSERT_EQ, builtin_assert_eq_inf},
     {nibi::kw::ASSERT_NEQ, builtin_assert_neq_inf},
     {nibi::kw::PUSH_FRONT, builtin_list_push_front_inf},
     {nibi::kw::PUSH_BACK, builtin_list_push_back_inf},
     {nibi::kw::POP_FRONT, builtin_list_pop_front_inf},
     {nibi::kw::POP_BACK, builtin_list_pop_back_inf},
     {nibi::kw::SPAWN, builtin_list_spawn_inf},
     {nibi::kw::ITER, builtin_list_iter_inf},
     {nibi::kw::AT, builtin_list_at_inf},
     {nibi::kw::LEN, builtin_common_len_inf},
     {nibi::kw::YIELD, builtin_common_yield_inf},
     {nibi::kw::LOOP, builtin_common_loop_inf},
     {nibi::kw::IF, builtin_common_if_inf},
     {nibi::kw::CLONE, builtin_common_clone_inf},
     {nibi::kw::PUT, builtin_common_put_inf},
     {nibi::kw::PUTLN, builtin_common_putln_inf},
     {nibi::kw::IMPORT, builtin_common_import_inf},
     {nibi::kw::USE, builtin_common_use_inf},
     {nibi::kw::EXIT, builtin_common_exit_inf},
     {nibi::kw::EVAL, builtin_common_eval_inf},
     {nibi::kw::QUOTE, builtin_common_quote_inf},
     {nibi::kw::NOP, builtin_common_nop_inf},
     {nibi::kw::BW_LSH, builtin_bitwise_lsh_inf},
     {nibi::kw::BW_RSH, builtin_bitwise_rsh_inf},
     {nibi::kw::BW_AND, builtin_bitwise_and_inf},
     {nibi::kw::BW_OR, builtin_bitwise_or_inf},
     {nibi::kw::BW_XOR, builtin_bitwise_xor_inf},
     {nibi::kw::BW_NOT, builtin_bitwise_not_inf},
     {nibi::kw::STR, builtin_cvt_string_inf},
     {nibi::kw::INT, builtin_cvt_int_inf},
     {nibi::kw::FLOAT, builtin_cvt_float_inf},
     {nibi::kw::SPLIT, builtin_cvt_split_inf},
     {nibi::kw::TYPE, builtin_reflect_type_inf}};

// Retrieve the map of symbols to function info structs
function_router_t &get_builtin_symbols_map() { return keyword_map; }

} // namespace builtins
} // namespace nibi
