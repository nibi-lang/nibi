#pragma once

#include "env.hpp"
#include "runtime.hpp"
#include "front/atom_view.hpp"

namespace runtime {

class core_c {
public:
  core_c() = delete;

  core_c(const std::string& origin)
    : _origin(origin){}


#warning "get rid of runtime::data_s. Mark-up cpp functions to take their specific arguments and build them out here rather than passing a list"

  object_ptr execute(
    const data_s& data,
    env_c &env);

  object_ptr evaluate(
    const data_s& data,
    atom_view::view_s *atom,
    env_c &env);
private:
  const std::string& _origin;
};

} // namespace
