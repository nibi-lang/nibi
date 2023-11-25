#include "runtime/builtins/builtins.hpp"
#include "runtime/runtime.hpp"
#include "runtime/object.hpp"
#include "runtime/core.hpp"

namespace builtins {

runtime::object_ptr assignment_let(
  atom_view::walker_c& walker,
  runtime::core_c &core,
  runtime::env_c &env) {

  walker.reset();
  walker.next();  // let
  auto var = core.object_from_view(
      walker.next())->to_string();

  if (env.get_env(var, true)) {
    return runtime::external_error(
      fmt::format("variable '{}' already defined",
        var));
  }

  runtime::object_ptr value = core.resolve(walker.next(), env);

  if (value->type == runtime::data_type_e::ERROR) {
    return value;
  }

  env.set(var, value);

  return runtime::success_value();
}

} // namespace
