#pragma once

#include "atoms.hpp"
#include "machine/instructions.hpp"
#include "tracer.hpp"

namespace front {

//! \brief Parses a "completed" atom list
//!        and pipes them to a given
//!        instruction receiver.
class parser_c final : public atom_receiver_if {
public:
  parser_c() = delete;
  parser_c(
    tracer_ptr& tracer,
    machine::instruction_receiver_if& ins_receiver);

  void on_error(error_s) override;
  void on_list(atom_list_t list) override;
  void on_top_list_complete() override;

private:
  tracer_ptr _tracer{nullptr};
  machine::instruction_receiver_if& _ins_receiver;
};

} // namespace
