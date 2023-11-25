#pragma once

#include "runtime/env.hpp"
#include "runtime/object.hpp"

namespace builtins {

static inline runtime::object_ptr success_value() {
  return runtime::allocate_object(
    runtime::object_c::boolean(true));
}

static inline runtime::object_ptr failure_value() {
  return runtime::allocate_object(
    runtime::object_c::boolean(false));
}

extern void populate_env(runtime::env_c &env);

} // namespace

