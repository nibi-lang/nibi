#pragma once

#include "runtime/object.hpp"
#include "runtime/core.hpp"
#include "runtime/env.hpp"

namespace runtime {

class object_ctx_c final : public complex_object_c {
public:
  object_ctx_c(
    const std::string& name,
    const core_c &core,
    env_c* parent=nullptr);
  ~object_ctx_c() = default;
  complex_object_c* clone() override;
  std::string to_string() override;
  std::size_t hash() const override;
  std::size_t length() const override;
  runtime::core_c core;
  runtime::env_c env;
  std::string name;
};

object_ctx_c::object_ctx_c(
  const std::string& name,
  const core_c &core_in,
  env_c* parent)
  : core(core_in.get_origin()),
    env(parent),
    name(name){

  // Users can't make names starting with comment char 
  env.set(";name", allocate_object((char*)name.data(), name.size()), true);
}

complex_object_c* object_ctx_c::clone() {
  object_ctx_c* no = new object_ctx_c(name, core_c(core.get_origin()));
  no->env = this->env.clone();
  return no;
}

std::string object_ctx_c::to_string() {
  return fmt::format("<env:{}>", name);
}

std::size_t object_ctx_c::hash() const {
  return std::hash<std::string>{}(fmt::format("<env:{}>", name)); 
}

std::size_t object_ctx_c::length() const {
  return env.count();
}

} // namespace
