#include "runtime/builtins/builtins.hpp"

namespace builtins {

extern runtime::object_ptr define_function(
  const runtime::object_list_t& params,
  runtime::env_c &env);

extern runtime::object_ptr assignment_let(
  const runtime::object_list_t& params,
  runtime::env_c &env);



void populate_env(runtime::env_c &env) {

  env.set("let", 
    runtime::allocate_object(
      (runtime::cpp_fn)assignment_let));

}


} // namespace
