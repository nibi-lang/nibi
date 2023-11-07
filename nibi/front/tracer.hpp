#pragma once

#include "front/types.hpp"
#include "machine/instructions.hpp"

#include <map>
#include <memory>

namespace front {

//! \brief Object that produces "tracers" for a given
//!        file. Tracers can be used to draw errors on
//!        the screen given a particular instruction id
//!        for a generated chunk of code
class traced_file_c {
public:
  traced_file_c() = delete;
  traced_file_c(const std::string& file)
    : _file_name(file) {}

  //! \brief A tracer that can be handed to the bytecode engine
  //!        for error reporting on chunks of bytecode
  class tracer_c final : public machine::instruction_error_handler_if {
  public:
    tracer_c(traced_file_c& parent)
      : _parent(parent){}

    //! \brief Register generated bytecode with position
    //!        information for later tracing
    void register_instruction(
      const machine::bytecode_idx_t& instruction_index,
      const pos_s& position) {
      _file_map[instruction_index] = position;
    }

    // From the interface, used to indicate errors
    void on_error(
      const machine::bytecode_idx_t& instruction_index,
      machine::execution_error_s err) override {

      auto target = _file_map.find(instruction_index);

      if (target == _file_map.end()) {
        err.message =
          std::string("Unable to load locator for instruction: ") +
          std::to_string(instruction_index) + 
          std::string("\n") +
          err.message;
        pos_s pos{0,0};
        _parent.trace_error(pos, err);
        return;
      }

      _parent.trace_error(target->second, err);
    }

  private:
    traced_file_c &_parent;
    std::map<machine::bytecode_idx_t, pos_s> _file_map;
  };

  //! \brief Create a new tracer for the current file
  std::shared_ptr<traced_file_c::tracer_c> get_tracer() {
    return std::make_unique<tracer_c>(*this);
  }

  //! \brief Trace an error in this traced_file_c
  void trace_error(const pos_s& pos,
                   machine::execution_error_s error);

  std::string get_name() const { return _file_name; }

private:
  std::string _file_name;
};

using tracer_ptr = std::shared_ptr<traced_file_c::tracer_c>;
using traced_file_ptr = std::shared_ptr<traced_file_c>;

constexpr auto allocate_traced_file = [](auto... args)
  -> traced_file_ptr {
    return std::make_shared<traced_file_c>(args...);
  };

constexpr auto allocate_tracer = [](auto... args)
  -> tracer_ptr {
    return std::make_shared<traced_file_c::tracer_c>(args...);
  };

} // namespace
