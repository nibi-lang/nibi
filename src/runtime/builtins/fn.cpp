#include "runtime/builtins/builtins.hpp"
#include "runtime/runtime.hpp"
#include "runtime/object.hpp"
#include "runtime/core.hpp"

namespace builtins {

using namespace runtime;

runtime::object_ptr define_function(
  atom_view::walker_c& walker,
  runtime::core_c &core,
  runtime::env_c &env) {

  // fn <name> (params) <body>...


  
  return runtime::failure_value();
}

} // namespace

