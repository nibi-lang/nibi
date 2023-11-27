#include "runtime/core.hpp"

namespace builtins {

#define BUILTIN_FN(name_, body_)\
  runtime::object_ptr name_( \
    atom_view::walker_c& walker,\
    runtime::core_c &core,\
    runtime::env_c &env) \
    body_

#define RETURN(with_) \
  walker.invalidate();\
  return with_;

#define ERROR_CHECK(item_) \
  if (item_->type == runtime::data_type_e::ERROR) { \
    walker.set_flag(); \
    RETURN(item_) \
  }

#define EVAL(to_) \
  walker.mark(); \
  auto to_ = core.resolve(walker.next(), env); \
  ERROR_CHECK(to_)

#define ADD_FN(val_, fn_) \
  env.set(val_, \
    runtime::allocate_object( \
      (runtime::cpp_fn)fn_));
 
BUILTIN_FN(builtin_define_function, {

  // fn <name> (params) <body>...
  
  RETURN(runtime::value::failure());
})

BUILTIN_FN(builtin_let, {
  auto var = core.object_from_view(
      walker.next())->to_string();

  if (env.get_env(var, true)) {
    RETURN(runtime::value::external_error(
      fmt::format("variable '{}' already defined",
        var)));
  }

  EVAL(value)

  env.set(var, value);

  RETURN(runtime::value::success())
})

BUILTIN_FN(builtin_set, {

  EVAL(target)
  EVAL(value)

  target->update_from(*(value.get()));

  RETURN(target)
})

BUILTIN_FN(builtin_resolve, {
  runtime::object_ptr result = runtime::value::none();
  while(walker.has_next()) {
    result = core.resolve(walker.next(), env);
    ERROR_CHECK(result)
  }
  return result;
})

BUILTIN_FN(builtin_assert, {
  EVAL(condition_result)

  if (condition_result->to_bool()) {
    RETURN(runtime::value::success())
  }

  std::string message = "Assertion failure";
  if (walker.has_next()) {
    message = fmt::format(
      "{}: {}", message, core.resolve(walker.next(), env)->to_string());
  }

  RETURN(runtime::value::external_error(message))
})

BUILTIN_FN(builtin_eq, {
  EVAL(lhs)
  EVAL(rhs)
  RETURN(
    runtime::allocate_object(
      lhs->equals(*rhs.get())))
})

BUILTIN_FN(builtin_lt, {
  EVAL(lhs)
  EVAL(rhs)
  RETURN(
    runtime::allocate_object(
      lhs->is_less_than(*rhs.get())))
})

BUILTIN_FN(builtin_gt, {
  EVAL(lhs)
  EVAL(rhs)
  RETURN(
    runtime::allocate_object(
      lhs->is_greater_than(*rhs.get())))
})

BUILTIN_FN(builtin_put, {
  while(walker.has_next()) {
    EVAL(result)
    fmt::print("{}", result->to_string());
  }
  RETURN(runtime::value::success())
})

#define STD_ARITH(op_)\
  EVAL(base) \
  if (!base->is_numeric()) {\
    walker.set_flag();\
    RETURN(runtime::value::external_error(\
      "Expected numeric type for operation"));\
  }\
  while(walker.has_next()) {\
    EVAL(item)\
    if (!item->is_numeric()) {\
      walker.set_flag();\
      RETURN(runtime::value::external_error(\
        "Expected numeric type for operation"));\
    }\
    base->update_from( (*base.get()) op_ (*item.get()));\
    ERROR_CHECK(base);\
  }\
  RETURN(base)

BUILTIN_FN(builtin_add, { STD_ARITH(+) })
BUILTIN_FN(builtin_sub, { STD_ARITH(-) })
BUILTIN_FN(builtin_mul, { STD_ARITH(*) })
BUILTIN_FN(builtin_div, {
  EVAL(base)

  if (!base->is_numeric()) {
    walker.set_flag();
    RETURN(runtime::value::external_error(
      "Expected numeric type for operation"));
  }

  while(walker.has_next()) {
    EVAL(item)
    if (!item->is_numeric()) {
      walker.set_flag();
      RETURN(runtime::value::external_error(
        "Expected numeric type for operation"));
    }
    
    if (item->to_integer() == 0) {
      walker.set_flag();
      RETURN(runtime::value::external_error(
        "Attempted division by 0"));
    }

    base->update_from( (*base.get()) / (*item.get()));;
    ERROR_CHECK(base);
  }
  RETURN(base)
})



void populate_env(runtime::env_c &env) {

  ADD_FN("put", builtin_put)
  ADD_FN("let", builtin_let)
  ADD_FN("set", builtin_set)
  ADD_FN("resolve", builtin_resolve)
  ADD_FN("assert", builtin_assert)
  ADD_FN("lt", builtin_lt)
  ADD_FN("gt", builtin_gt)
  ADD_FN("eq", builtin_eq)
  ADD_FN("+", builtin_add)
  ADD_FN("-", builtin_sub)
  ADD_FN("/", builtin_div)
  ADD_FN("*", builtin_mul)

}

} // namespace
