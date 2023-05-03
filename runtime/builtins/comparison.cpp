#include <iostream>

#include "arithmetic_helpers.hpp"
#include "runtime/builtins/builtins.hpp"
#include "runtime/builtins/cpp_macros.hpp"
#include "runtime/cell.hpp"
#include "runtime/runtime.hpp"

#define PERFORM_OP_ALLOW_STRING(___op)                                         \
  {                                                                            \
    switch (target_type) {                                                     \
    case cell_type_e::INTEGER: {                                               \
      auto r =                                                                 \
          ALLOCATE_CELL((int64_t)(lhs.as_integer() ___op rhs.as_integer()));   \
      r->locator = locator;                                                    \
      return r;                                                                \
    }                                                                          \
    case cell_type_e::DOUBLE: {                                                \
      auto r =                                                                 \
          ALLOCATE_CELL((int64_t)(lhs.as_double() ___op rhs.as_double()));     \
      r->locator = locator;                                                    \
      return r;                                                                \
    }                                                                          \
    case cell_type_e::STRING: {                                                \
      auto r =                                                                 \
          ALLOCATE_CELL((int64_t)(lhs.as_string() ___op rhs.to_string()));     \
      r->locator = locator;                                                    \
      return r;                                                                \
    }                                                                          \
    default: {                                                                 \
      auto r =                                                                 \
          ALLOCATE_CELL((int64_t)(lhs.to_string() ___op rhs.to_string()));     \
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
          ALLOCATE_CELL((int64_t)(lhs.as_integer() ___op rhs.as_integer()));   \
      r->locator = locator;                                                    \
      return r;                                                                \
    }                                                                          \
    case cell_type_e::DOUBLE: {                                                \
      auto r =                                                                 \
          ALLOCATE_CELL((int64_t)(lhs.as_double() ___op rhs.as_double()));     \
      r->locator = locator;                                                    \
      return r;                                                                \
    }                                                                          \
    default:                                                                   \
      throw runtime_c::exception_c(                                            \
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
};

cell_ptr perform_op(locator_ptr locator, op_e op, cell_c &lhs, cell_c &rhs,
                    bool enforce_numeric = true) {
  if (enforce_numeric) {
    if (!lhs.is_numeric()) {
      throw runtime_c::exception_c(
          "Expected numeric value, got " +
              std::string(cell_type_to_string(lhs.type)),
          lhs.locator);
    }
    if (!rhs.is_numeric()) {
      throw runtime_c::exception_c(
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
  }

  throw runtime_c::exception_c("Unknown comparison operator", lhs.locator);
}
} // namespace

cell_ptr builtin_fn_comparison_eq(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("eq", ==, 3)
  return perform_op(list.front()->locator, op_e::EQ,
                    *list_get_nth_arg(1, list, env),
                    *list_get_nth_arg(2, list, env), false);
}
cell_ptr builtin_fn_comparison_neq(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("neq", ==, 3)
  return perform_op(list.front()->locator, op_e::NEQ,
                    *list_get_nth_arg(1, list, env),
                    *list_get_nth_arg(2, list, env), false);
}
cell_ptr builtin_fn_comparison_lt(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("<", ==, 3)
  return perform_op(list.front()->locator, op_e::LT,
                    *list_get_nth_arg(1, list, env),
                    *list_get_nth_arg(2, list, env));
}
cell_ptr builtin_fn_comparison_gt(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE(">", ==, 3)
  return perform_op(list.front()->locator, op_e::GT,
                    *list_get_nth_arg(1, list, env),
                    *list_get_nth_arg(2, list, env));
}
cell_ptr builtin_fn_comparison_lte(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE("<=", ==, 3)
  return perform_op(list.front()->locator, op_e::LTE,
                    *list_get_nth_arg(1, list, env),
                    *list_get_nth_arg(2, list, env));
}
cell_ptr builtin_fn_comparison_gte(cell_list_t &list, env_c &env) {
  LIST_ENFORCE_SIZE(">=", ==, 3)
  return perform_op(list.front()->locator, op_e::GTE,
                    *list_get_nth_arg(1, list, env),
                    *list_get_nth_arg(2, list, env));
}

} // namespace builtins