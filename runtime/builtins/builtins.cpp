#include <iostream>

#include "runtime/builtins/builtins.hpp"
#include "runtime/cell.hpp"

namespace builtins {

// All functions in cells are tagged wit their type, their symbol, and contain
// a pointer to their functionality. Here we declare them for all builtin
// functions so all cells point to the same instantiation of the function info
// struct
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
static function_info_s builtin_assignment_inf = {
    ":=", builtin_fn_env_assignment, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_drop_inf = {
    "drop", builtin_fn_env_drop, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_assert_inf = {
    "true!", builtin_fn_assert_true, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_is_nil_inf = {
    "nil?", builtin_fn_check_nil, function_type_e::BUILTIN_CPP_FUNCTION};
static function_info_s builtin_dbg_inf = {
    "dbg", builtin_fn_debug_dbg, function_type_e::BUILTIN_CPP_FUNCTION};

// This map is used to look up the function info struct for a given symbol
static std::unordered_map<std::string, function_info_s> keyword_map = {
    {"+", builtin_add_inf},         {"-", builtin_sub_inf},
    {"/", builtin_div_inf},         {"*", builtin_mul_inf},
    {"%", builtin_mod_inf},         {"**", builtin_pow_inf},
    {":=", builtin_assignment_inf}, {"drop", builtin_drop_inf},
    {"true!", builtin_assert_inf},  {"nil?", builtin_is_nil_inf},
    {"dbg", builtin_dbg_inf}
};

// Retrieve the map of symbols to function info structs
std::unordered_map<std::string, function_info_s> &get_builtin_symbols_map() {
  return keyword_map;
}

} // namespace builtins