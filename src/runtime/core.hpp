#pragma once

#include "env.hpp"
#include "object.hpp"
#include "runtime.hpp"
#include "front/atom_view.hpp"

namespace runtime {

class core_c {
public:
  core_c() = delete;

  core_c(const std::string& origin)
    : _origin(origin){}

  object_ptr execute(
    uint8_t* data,
    const std::size_t& len,
    env_c &env);

  object_ptr evaluate(
    atom_view::walker_c& walker,
    env_c &env);

  object_ptr evaluate_list(object_list_t& list, env_c &env);
private:
  const std::string& _origin;

  object_ptr object_from_view(
      atom_view::view_s* view);
  object_ptr prep_and_call_cpp_fn(
      cpp_fn &fn, atom_view::walker_c& walker, env_c &env);
};

} // namespace
