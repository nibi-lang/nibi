#pragma once

#include "atoms.hpp"
#include "tracer.hpp"
#include "machine/instructions.hpp"
#include "runtime/context.hpp"

namespace front {

//! \brief Parses a "completed" atom list
//!        and pipes them to a given
//!        instruction receiver.
class parser_c final : public atom_receiver_if {
public:
  parser_c() = delete;
  parser_c(
    tracer_ptr ptr,
    machine::instruction_receiver_if& ins_receiver);

  void on_error(error_s) override;
  void on_list(atom_list_t list) override;
  void on_top_list_complete() override;

private:
  struct block_s {
    machine::bytes_t data;
    machine::bytecode_idx_t bc_index{0};
    machine::bytecode_idx_t bump() { return bc_index++; }
  };

  struct state_s {
    block_s instruction_block;
    size_t list_depth{0};
    bool function_symbol_expected{false};
    void reset() {
      instruction_block.data.clear();
      instruction_block.bc_index = 0;
      list_depth = 0;
      function_symbol_expected = false;
    }
  };

  state_s state;

  tracer_ptr _tracer{nullptr};
  machine::instruction_receiver_if& _ins_receiver;

  void decompose(atom_ptr&);
  void decompose_symbol(const meta_e&, const std::string&);
};

} // namespace
