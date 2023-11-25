#include "runtime/builtins/builtins.hpp"
#include "runtime/core.hpp"

namespace builtins {

extern runtime::object_ptr builtin_define_function(
  atom_view::walker_c& walker,
  runtime::core_c &core,
  runtime::env_c &env);

runtime::object_ptr builtin_let(
  atom_view::walker_c& walker,
  runtime::core_c &core,
  runtime::env_c &env) {

  auto var = core.object_from_view(
      walker.next())->to_string();

  if (env.get_env(var, true)) {
    walker.invalidate();
    return runtime::value::external_error(
      fmt::format("variable '{}' already defined",
        var));
  }

  runtime::object_ptr value = core.resolve(walker.next(), env);

  if (value->type == runtime::data_type_e::ERROR) {
    walker.invalidate();
    return value;
  }

  env.set(var, value);

  walker.invalidate();
  return runtime::value::success();
}

runtime::object_ptr builtin_set(
  atom_view::walker_c& walker,
  runtime::core_c &core,
  runtime::env_c &env) {

  auto target = core.resolve(walker.next(), env);
  if (target->type == runtime::data_type_e::ERROR) {
    walker.invalidate();
    return target;
  }

  auto value = core.resolve(walker.next(), env);
  if (value->type == runtime::data_type_e::ERROR) {
    walker.invalidate();
    return value;
  }

  target->update_from(*(value.get()));

  walker.invalidate();
  return target;
}

runtime::object_ptr builtin_resolve(
  atom_view::walker_c& walker,
  runtime::core_c &core,
  runtime::env_c &env) {
  
  runtime::object_ptr result = runtime::value::none();
  while(walker.has_next()) {
    result = core.resolve(walker.next(), env);
  }
  return result;
}

runtime::object_ptr builtin_assert(
  atom_view::walker_c& walker,
  runtime::core_c &core,
  runtime::env_c &env) {
  
  auto condition_result = core.resolve(walker.next(), env);
  if (condition_result->type == runtime::data_type_e::ERROR) {
    walker.invalidate();
    return condition_result;
  }

  if (condition_result->to_bool()) {
    walker.invalidate();
    return runtime::value::success();
  }

  std::string message = "Assertion failure";
  if (walker.has_next()) {
    message = fmt::format(
      "{}: {}", message, core.resolve(walker.next(), env)->to_string());
  }

  walker.invalidate();
  return runtime::value::external_error(message);
}

void populate_env(runtime::env_c &env) {

  env.set("let", 
    runtime::allocate_object(
      (runtime::cpp_fn)builtin_let));

  env.set("set", 
    runtime::allocate_object(
      (runtime::cpp_fn)builtin_set));

  env.set("resolve", 
    runtime::allocate_object(
      (runtime::cpp_fn)builtin_resolve));

  env.set("assert", 
    runtime::allocate_object(
      (runtime::cpp_fn)builtin_assert));
}

} // namespace
