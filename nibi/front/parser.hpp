#pragma once

#include "atoms.hpp"
#include "tracer.hpp"
#include "machine/instructions.hpp"
#include "runtime/context.hpp"
#include "builtins.hpp"

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
    size_t instruction_index{0};
    size_t bump(size_t count=1) {
      size_t v = instruction_index;
      instruction_index += count;
      return v;
    }
  };

  struct state_s {
    block_s instruction_block;
    size_t list_depth{0};
    bool function_symbol_expected{false};
    atom_list_t* current_list{nullptr};
    size_t current_list_idx{1};
    void reset() {
      instruction_block.data.clear();
      instruction_block.instruction_index = 0;
      list_depth = 0;
      function_symbol_expected = false;
      current_list_idx = 1;
      current_list = nullptr;
    }

    inline atom_ptr& get_current() const {
      return (*current_list)[current_list_idx];
    }

    inline void next() { current_list_idx++; }

    inline bool good() { return current_list_idx < current_list->size(); }

    inline bool has_next() {
      if (!current_list) { return false; }
      if (current_list_idx + 1 >= current_list->size()) { return false; }
      return true;
    }

    inline bool is_next(const meta_e& x) {
      if (!has_next()) { return false; }
      return (*current_list)[current_list_idx + 1]->meta == x;
    }
  };

  state_s state;

  tracer_ptr _tracer{nullptr};
  machine::instruction_receiver_if& _ins_receiver;
  front::builtins::builtin_map_t* _builtin_map{nullptr};

  void decompose(atom_ptr&, bool req_exec=false);
  void decompose_symbol(const meta_e&, const std::string&, const pos_s& pos, bool req_exec=false);

  void build_accessor(const std::string&, bool req_exec);
};

} // namespace
