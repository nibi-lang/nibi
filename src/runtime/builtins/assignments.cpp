#include "runtime/builtins/builtins.hpp"
#include "runtime/runtime.hpp"
#include "runtime/object.hpp"

namespace builtins {

runtime::object_ptr assignment_let(
  const runtime::object_list_t& params,
  runtime::env_c &env) {

  fmt::print("Got a let list of size {}\n",
    params.size());

  // let <id> <list|raw>
  //

  for(auto &&o : params) {

    fmt::print("{}\n", o->to_string());
  }

  std::string target_name = params[0]->to_string();

  if (env.get_env(target_name, true)) {
    return runtime::external_error(
      fmt::format("variable '{}' already defined",
        target_name));
  }

  

  return runtime::failure_value();
}

} // namespace
