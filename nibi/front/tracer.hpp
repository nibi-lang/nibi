#pragma once

#include "front/types.hpp"
#include "machine/instructions.hpp"

#include <map>
#include <memory>
#include <fmt/format.h>

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

    struct group_s {
      machine::bytecode_idx_t id{0};
      pos_s pos;
    };

    //! \brief Register generated bytecode with position
    //!        information for later tracing
    void register_instruction(
      const machine::bytecode_idx_t& instruction_index,
      const pos_s& position) {
      //_file_map[instruction_index] = position;
      _file_group.push_back({instruction_index, position});
    }

    void on_error(const error_s& err) {
      _parent.trace_error(err.pos, err.message, true);
    }

    // From the interface, used to indicate errors
    void on_error(
      const machine::bytecode_idx_t& instruction_index,
      machine::execution_error_s err) override {


      group_s *target{nullptr};
      for(size_t i = 0; i < _file_group.size(); ++i) {
        if (_file_group[i].id == instruction_index) {
          fmt::print("Found instruction exactly\n");
          target = &_file_group[i];
          break;
        }
        if (_file_group[i].id > instruction_index) {
          fmt::print("id {} exceeds {}, assuming previous instruction's ownership",
            _file_group[i].id, instruction_index);
          break;
        }
        target = &_file_group[i];
      }

      fmt::print("Tracer suspects instruction is '{}' at line:{}, col:{}\n",
          target->id, target->pos.line, target->pos.col);

      /*
      auto target = _file_map.find(instruction_index);
      if (target == _file_map.end()) {
        // TODO: change this to show only on debug builds that ask for it
        for(auto [x, y] : _file_map) {
          fmt::print("idx: {} | pos {}, {}\n", x, y.line, y.col);
        }
        err.message = fmt::format("Unable to load locator for instructions: {}\n", instruction_index);
        pos_s pos{0,0};
        _parent.trace_error(pos, err.message, true);
        return;
      }
      */
      _parent.trace_error(target->pos, err.message, err.fatal);
    }

  private:
    traced_file_c &_parent;
    std::map<machine::bytecode_idx_t, pos_s> _file_map;
    std::vector<group_s> _file_group;
  };

  //! \brief Create a new tracer for the current file
  std::shared_ptr<traced_file_c::tracer_c> get_tracer() {
    return std::make_unique<tracer_c>(*this);
  }

  //! \brief Trace an error in this traced_file_c
  void trace_error(const pos_s& pos,
                   const std::string& message,
                   bool is_fatal);

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
