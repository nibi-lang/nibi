#pragma once

#include "front/tracer.hpp"
#include "machine/memory_core.hpp"
#include <unordered_map>

namespace runtime {


class context_c {
public:

  machine::memory_core_c& get_memory_core() {
    return _memory;
  }

  void add_traced_file(
    front::traced_file_ptr traced) {
    _active_traced_files[traced->get_name()] = traced;
  }

  void add_tracer(
    const std::string& origin,
    front::tracer_ptr tracer) {
    _active_tracers[origin] = std::move(tracer);
  }

  

private:

  machine::memory_core_c _memory; 

  std::unordered_map<std::string,
    front::traced_file_ptr> _active_traced_files;
  std::unordered_map<std::string,
    front::tracer_ptr> _active_tracers;
};

} // namespace
