#pragma once

#include "runtime/object.hpp"

namespace runtime {

static inline runtime::object_ptr success_value() {
  return runtime::allocate_object(
    runtime::object_c::boolean(true));
}

static inline runtime::object_ptr failure_value() {
  return runtime::allocate_object(
    runtime::object_c::boolean(false));
}

static inline runtime::object_ptr external_error(
  const std::string& message) {
  return runtime::allocate_object(
    runtime::object_error_c(message));
}

/*
namespace external {

enum class c_types_e {
  VOID,
  U8, U16, U32, U64,
  I8, I16, I32, I64,
  F32, F64,
  SIZE_T,
  C_STR,
};

struct type_info_s {
  c_types_e type{c_types_e::VOID};
  bool permit_implicit_cast{false};
  bool is_ptr{false};
};

struct typed_callable_s {
  type_info_s return_value;
  std::vector<type_info_s> param_types;
  std::string name;
  void* fn{nullptr};
};

} // namespace
*/

} // namespace 

