#pragma once
#include "front/tracer.hpp"
#include "machine/memory_core.hpp"
#include <unordered_map>
#include <vector>
#include <optional>

namespace runtime {

//! \brief The runtime context for an executing process
//! \note  The runtime context stores the primary memory
//!        core shared by all processes, instruction tracing
//!        information, and other information executing 
//!        processes my require to function
class context_c {
public:
  context_c(
    std::vector<std::string> &program_args,
    std::vector<std::string> &stdin_pipe)
    : _args(program_args),
      _stdin_pipe(stdin_pipe){}

  machine::memory_core_c& get_memory_core() {
    return _memory;
  }

  std::vector<std::string> &get_program_args() const {
    return _args;
  }

  std::vector<std::string> &get_program_stdin_pipe() const {
    return _stdin_pipe;
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
  std::vector<std::string> &_args;
  std::vector<std::string> &_stdin_pipe;

  machine::memory_core_c _memory; 

  std::unordered_map<std::string,
    front::traced_file_ptr> _active_traced_files;
  std::unordered_map<std::string,
    front::tracer_ptr> _active_tracers;
};

} // namespace
