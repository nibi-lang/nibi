#pragma once

#include "atoms.hpp"
#include "tracer.hpp"
#include "machine/instructions.hpp"
#include "runtime/context.hpp"
#include "forge.hpp"

namespace front {

//! \brief Parses a "completed" atom list
//!        and pipes them to a given
//!        instruction receiver.
class generator_c final : public atom_receiver_if {
public:
  generator_c() = delete;
  generator_c(
    tracer_ptr ptr,
    machine::instruction_receiver_if& ins_receiver);

  void on_error(error_s) override;
  void on_list(atom_list_t list) override;
  void on_top_list_complete() override;

private:


  machine::bytes_t instruction_set;

  void merge_and_register_instructions(forge::block_list_t&);

  tracer_ptr _tracer{nullptr};
  machine::instruction_receiver_if& _ins_receiver;

  std::vector<atom_list_t> _lists;

  std::map<std::string, 
    std::function<forge::block_list_t(atom_list_t&)>> _decomp_map;

  void generate();
  [[nodiscard]] forge::block_list_t decompose_atom_list(
      atom_list_t& list);
  void standard_decompose(
      forge::block_list_t&, atom_ptr&, bool req_exec=false);
  void decompose_symbol(
      forge::block_list_t&, atom_ptr&, bool req_exec=false);

  forge::block_list_t decompose_assert(atom_list_t&);
  forge::block_list_t decompose_let(atom_list_t&);
  forge::block_list_t decompose_set(atom_list_t&);
  forge::block_list_t decompose_use(atom_list_t&);
  forge::block_list_t decompose_dbg(atom_list_t&);
  forge::block_list_t decompose_if(atom_list_t&);
     // const meta_e&, const std::string&, const pos_s& pos, bool req_exec=false);
};

} // namespace
