#include "runtime/builtins/builtins.hpp"
#include "runtime/core.hpp"

namespace builtins {

extern runtime::object_ptr define_function(
  atom_view::walker_c& walker,
  runtime::core_c &core,
  runtime::env_c &env);

extern runtime::object_ptr assignment_let(
  atom_view::walker_c& walker,
  runtime::core_c &core,
  runtime::env_c &env);



void populate_env(runtime::env_c &env) {

  env.set("let", 
    runtime::allocate_object(
      (runtime::cpp_fn)assignment_let));

}


} // namespace
