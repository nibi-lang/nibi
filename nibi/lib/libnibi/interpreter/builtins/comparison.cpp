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
    if (lhs.is_integer()) {                                                   \
      auto r =                                                                 \
          allocate_cell((int64_t)(lhs.as_integer() ___op rhs.to_integer()));   \
      r->locator = locator;                                                    \
      return std::move(r);                                                     \
    }                                                                          \
    else if (lhs.is_float()) {                                                \
      auto r =                                                                 \
          allocate_cell((int64_t)(lhs.as_double() ___op rhs.to_double()));     \
      r->locator = locator;                                                    \
      return std::move(r);                                                     \
    }                                                                          \
    else if (lhs.type == cell_type_e::STRING) {                               \
      auto r =                                                                 \
          allocate_cell((int64_t)(lhs.as_string() ___op rhs.to_string()));     \
      r->locator = locator;                                                    \
      return std::move(r);                                                     \
    }                                                                          \
    else {                                                                     \
      auto r =                                                                 \
          allocate_cell((int64_t)(lhs.to_string() ___op rhs.to_string()));     \
      r->locator = locator;                                                    \
      return std::move(r);                                                     \
    }                                                                          \
  }

#define PERFORM_OP_NO_STRING(___op)                                            \
  {                                                                            \
    if (lhs.is_integer()) {                                                   \
      auto r =                                                                 \
          allocate_cell((int64_t)(lhs.as_integer() ___op rhs.to_integer()));   \
      r->locator = locator;                                                    \
      return std::move(r);                                                     \
    }                                                                          \
    else if (lhs.is_float()) {                                                \
      auto r =                                                                 \
          allocate_cell((int64_t)(lhs.as_double() ___op rhs.to_double()));     \
      r->locator = locator;                                                    \
      return std::move(r);                                                     \
    }                                                                          \
    else {                                                                     \
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

cell_ptr builtin_fn_comparison_eq(cell_processor_if &ci, cell_list_t &list,
                                  env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::EQ, ==, 3)
  return std::move(perform_op(list.front()->locator, op_e::EQ,
                              *ci.process_cell(list[1], env),
                              *ci.process_cell(list[2], env), false));
}
cell_ptr builtin_fn_comparison_neq(cell_processor_if &ci, cell_list_t &list,
                                   env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::NEQ, ==, 3)
  return std::move(perform_op(list.front()->locator, op_e::NEQ,
                              *ci.process_cell(list[1], env),
                              *ci.process_cell(list[2], env), false));
}
cell_ptr builtin_fn_comparison_lt(cell_processor_if &ci, cell_list_t &list,
                                  env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::LT, ==, 3)
  return std::move(perform_op(list.front()->locator, op_e::LT,
                              *ci.process_cell(list[1], env),
                              *ci.process_cell(list[2], env)));
}
cell_ptr builtin_fn_comparison_gt(cell_processor_if &ci, cell_list_t &list,
                                  env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::GT, ==, 3)
  return std::move(perform_op(list.front()->locator, op_e::GT,
                              *ci.process_cell(list[1], env),
                              *ci.process_cell(list[2], env)));
}
cell_ptr builtin_fn_comparison_lte(cell_processor_if &ci, cell_list_t &list,
                                   env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::LTE, ==, 3)
  return std::move(perform_op(list.front()->locator, op_e::LTE,
                              *ci.process_cell(list[1], env),
                              *ci.process_cell(list[2], env)));
}
cell_ptr builtin_fn_comparison_gte(cell_processor_if &ci, cell_list_t &list,
                                   env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::GTE, ==, 3)
  return std::move(perform_op(list.front()->locator, op_e::GTE,
                              *ci.process_cell(list[1], env),
                              *ci.process_cell(list[2], env)));
}
cell_ptr builtin_fn_comparison_and(cell_processor_if &ci, cell_list_t &list,
                                   env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::AND, ==, 3)
  return std::move(perform_op(list.front()->locator, op_e::AND,
                              *ci.process_cell(list[1], env),
                              *ci.process_cell(list[2], env)));
}
cell_ptr builtin_fn_comparison_or(cell_processor_if &ci, cell_list_t &list,
                                  env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::OR, ==, 3)
  return std::move(perform_op(list.front()->locator, op_e::OR,
                              *ci.process_cell(list[1], env),
                              *ci.process_cell(list[2], env)));
}

cell_ptr builtin_fn_comparison_not(cell_processor_if &ci, cell_list_t &list,
                                   env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::NOT, ==, 2)
  auto it = list.begin();
  std::advance(it, 1);
  auto item_to_negate = ci.process_cell(*it, env, true);
  auto value = item_to_negate->to_integer();
  auto result = allocate_cell((int64_t)(!value));
  result->locator = list[0]->locator;
  return std::move(result);
}

} // namespace builtins
} // namespace nibi
