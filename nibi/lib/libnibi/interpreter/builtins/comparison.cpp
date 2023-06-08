#include <iostream>

#include "arithmetic_helpers.hpp"
#include "interpreter/builtins/builtins.hpp"
#include "interpreter/interpreter.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/keywords.hpp"
#include "macros.hpp"

namespace nibi {

#define PERFORM_OP_ALLOW_STRING(___op)                                         \
  {                                                                            \
    switch (target_type) {                                                     \
    case cell_type_e::INTEGER: {                                               \
      auto r =                                                                 \
          allocate_cell((int64_t)(lhs.as_integer() ___op rhs.to_integer()));   \
      r->locator = locator;                                                    \
      return r;                                                                \
    }                                                                          \
    case cell_type_e::DOUBLE: {                                                \
      auto r =                                                                 \
          allocate_cell((int64_t)(lhs.as_double() ___op rhs.to_double()));     \
      r->locator = locator;                                                    \
      return r;                                                                \
    }                                                                          \
    case cell_type_e::STRING: {                                                \
      auto r =                                                                 \
          allocate_cell((int64_t)(lhs.as_string() ___op rhs.to_string()));     \
      r->locator = locator;                                                    \
      return r;                                                                \
    }                                                                          \
    default: {                                                                 \
      auto r =                                                                 \
          allocate_cell((int64_t)(lhs.to_string() ___op rhs.to_string()));     \
      r->locator = locator;                                                    \
      return r;                                                                \
    }                                                                          \
    }                                                                          \
  }

#define PERFORM_OP_NO_STRING(___op)                                            \
  {                                                                            \
    switch (target_type) {                                                     \
    case cell_type_e::INTEGER: {                                               \
      auto r =                                                                 \
          allocate_cell((int64_t)(lhs.as_integer() ___op rhs.to_integer()));   \
      r->locator = locator;                                                    \
      return r;                                                                \
    }                                                                          \
    case cell_type_e::DOUBLE: {                                                \
      auto r =                                                                 \
          allocate_cell((int64_t)(lhs.as_double() ___op rhs.to_double()));     \
      r->locator = locator;                                                    \
      return r;                                                                \
    }                                                                          \
    default:                                                                   \
      throw interpreter_c::exception_c(                                        \
          "Expected numeric value, got " +                                     \
              std::string(cell_type_to_string(lhs.type)),                      \
          lhs.locator);                                                        \
    }                                                                          \
  }

namespace builtins {

namespace {
enum class op_e {
  EQ,
  NEQ,
  LT,
  GT,
  LTE,
  GTE,
  AND,
  OR,
};

cell_ptr perform_op(locator_ptr locator, op_e op, cell_c &lhs, cell_c &rhs,
                    bool enforce_numeric = true) {
  if (enforce_numeric) {
    if (!lhs.is_numeric()) {
      throw interpreter_c::exception_c(
          "Expected numeric value, got " +
              std::string(cell_type_to_string(lhs.type)),
          lhs.locator);
    }
    if (!rhs.is_numeric()) {
      throw interpreter_c::exception_c(
          "Expected numeric value, got " +
              std::string(cell_type_to_string(rhs.type)),
          rhs.locator);
    }
  }

  cell_type_e target_type = lhs.type;
  switch (op) {
  case op_e::EQ:
    PERFORM_OP_ALLOW_STRING(==)
  case op_e::NEQ:
    PERFORM_OP_ALLOW_STRING(!=)
  case op_e::LT:
    PERFORM_OP_NO_STRING(<)
  case op_e::GT:
    PERFORM_OP_NO_STRING(>)
  case op_e::LTE:
    PERFORM_OP_NO_STRING(<=)
  case op_e::GTE:
    PERFORM_OP_NO_STRING(>=)
  case op_e::AND:
    PERFORM_OP_NO_STRING(&&)
  case op_e::OR:
    PERFORM_OP_NO_STRING(||)
  }

  throw interpreter_c::exception_c("Unknown comparison operator", lhs.locator);
}
} // namespace

cell_ptr builtin_fn_comparison_eq(interpreter_c &ci, cell_list_t &list,
                                  env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::EQ, ==, 3)
  return perform_op(list.front()->locator, op_e::EQ,
                    *ci.execute_cell(list[1], env),
                    *ci.execute_cell(list[2], env), false);
}
cell_ptr builtin_fn_comparison_neq(interpreter_c &ci, cell_list_t &list,
                                   env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::NEQ, ==, 3)
  return perform_op(list.front()->locator, op_e::NEQ,
                    *ci.execute_cell(list[1], env),
                    *ci.execute_cell(list[2], env), false);
}
cell_ptr builtin_fn_comparison_lt(interpreter_c &ci, cell_list_t &list,
                                  env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::LT, ==, 3)
  return perform_op(list.front()->locator, op_e::LT,
                    *ci.execute_cell(list[1], env),
                    *ci.execute_cell(list[2], env));
}
cell_ptr builtin_fn_comparison_gt(interpreter_c &ci, cell_list_t &list,
                                  env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::GT, ==, 3)
  return perform_op(list.front()->locator, op_e::GT,
                    *ci.execute_cell(list[1], env),
                    *ci.execute_cell(list[2], env));
}
cell_ptr builtin_fn_comparison_lte(interpreter_c &ci, cell_list_t &list,
                                   env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::LTE, ==, 3)
  return perform_op(list.front()->locator, op_e::LTE,
                    *ci.execute_cell(list[1], env),
                    *ci.execute_cell(list[2], env));
}
cell_ptr builtin_fn_comparison_gte(interpreter_c &ci, cell_list_t &list,
                                   env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::GTE, ==, 3)
  return perform_op(list.front()->locator, op_e::GTE,
                    *ci.execute_cell(list[1], env),
                    *ci.execute_cell(list[2], env));
}
cell_ptr builtin_fn_comparison_and(interpreter_c &ci, cell_list_t &list,
                                   env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::AND, ==, 3)
  return perform_op(list.front()->locator, op_e::AND,
                    *ci.execute_cell(list[1], env),
                    *ci.execute_cell(list[2], env));
}
cell_ptr builtin_fn_comparison_or(interpreter_c &ci, cell_list_t &list,
                                  env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::OR, ==, 3)
  return perform_op(list.front()->locator, op_e::OR,
                    *ci.execute_cell(list[1], env),
                    *ci.execute_cell(list[2], env));
}

cell_ptr builtin_fn_comparison_not(interpreter_c &ci, cell_list_t &list,
                                   env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::NOT, ==, 2)
  auto it = list.begin();
  std::advance(it, 1);
  auto item_to_negate = ci.execute_cell(*it, env, true);
  auto value = item_to_negate->to_integer();
  return allocate_cell((int64_t)(!value));
}

} // namespace builtins
} // namespace nibi
