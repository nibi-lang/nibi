#include "runtime/builtins/builtins.hpp"
#include "runtime/runtime.hpp"
#include "runtime/object.hpp"

namespace builtins {

using namespace runtime;

runtime::object_ptr define_function(
  const runtime::object_list_t& params,
  runtime::env_c &env) {

  // fn <name> (params) <body>...


  
  return runtime::failure_value();
}

} // namespace

