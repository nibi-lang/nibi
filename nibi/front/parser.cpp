#include "parser.hpp"
#include "forge.hpp"
#include "machine/instructions.hpp"
#include "machine/object.hpp"
#include "front/builtins.hpp"

#include <iostream>

namespace front {

parser_c::parser_c(
  tracer_ptr tracer,
  machine::instruction_receiver_if& ins_receiver)
  : _tracer(tracer),
    _ins_receiver(ins_receiver) {
  _builtin_map = &builtins::get_builtins();
}

void parser_c::on_error(error_s error) {
  std::cout << "Parser received an error report"
            << std::endl;
}

void parser_c::on_top_list_complete() {
  // Note: 
  // Engine _must_ be instructed to copy any instruction over
  // that may be referenced later

  // Tracers are stored in an active runtime context. As long as the context is alive,
  // and the tracer was properly registered with the context,
  // the reference to the shared value will live on. We _could_ update the machine
  // to take a tracer, but for the sake of loose coupling, we are using the interface
  // provided by machine that the tracer utilizes
  _ins_receiver.handle_instructions(
    state.instruction_block.data,
    *_tracer.get());

  // If we want to show output on repl, we could do so by grabing
  // the value here.

  _ins_receiver.reset_instruction_handling();

  state.reset(); 
}

void parser_c::on_list(atom_list_t list) {

  std::cout << "Parser got an atom list : "
            << list.size()
            << std::endl;
  print_list(list);

  if (list.empty()) return;

  auto& block = state.instruction_block;
  auto& data = block.data;

  _tracer->register_instruction(
      state.instruction_block.bump(), list[0]->pos);
  forge::load_instruction(data, machine::ins_id_e::PUSH_PROC_FRAME);

  for(size_t i = 1; i < list.size(); i++) {
    decompose(list[i]);
  }

  _tracer->register_instruction(
      state.instruction_block.bump(), list[0]->pos);
  decompose(list[0]);

  state.list_depth++;

  _tracer->register_instruction(
      state.instruction_block.bump(), list.back()->pos);
  forge::load_instruction(data, machine::ins_id_e::POP_PROC_FRAME);
}

void parser_c::decompose(atom_ptr& atom) {

  _tracer->register_instruction(
      state.instruction_block.bump(), atom->pos);

  auto& block = state.instruction_block;
  auto& data = block.data;

  switch(atom->atom_type) {
    case atom_type_e::UNDEFINED: {
      // TODO: Burn everything down
      break;
    }
    case atom_type_e::INTEGER:
      return forge::load_raw<int64_t>(
        data,
        std::stoull(atom->data.c_str()));
    case atom_type_e::REAL:
      return forge::load_raw<double>(
        data,
        std::stod(atom->data.c_str()));
    case atom_type_e::STRING:
      return forge::load_instruction(
        data,
        machine::ins_id_e::LOAD_STRING,
        machine::tools::pack_string(atom->data));
    case atom_type_e::LOAD_ARG:
      return forge::load_instruction(
        data,
        machine::ins_id_e::LOAD_RESULT);
    case atom_type_e::SYMBOL:
      return decompose_symbol(
        atom->meta,
        atom->data,
        atom->pos);
  }
}

#define PARSER_LOAD_AND_EXPECT_GTE_N(op, n) \
 forge::load_instruction( \
     data, machine::ins_id_e::EXPECT_GTE_N_ARGS, machine::tools::pack<uint8_t>(2)); \
  _tracer->register_instruction(\
      state.instruction_block.bump(), pos); \
 return forge::load_instruction(data, op);

void parser_c::decompose_symbol(
  const meta_e& meta, const std::string& str, const pos_s& pos) {

  auto& block = state.instruction_block;
  auto& data = block.data;

  switch (meta) {
    default: {
      std::cerr << "META " << (int)meta << " : " << str
        << " is not yet implemented " << std::endl;

      std::cout 
        << "Continuing with default behavior (assuming identifier)\n";
      [[fallthrough]];
    }
    case meta_e::IDENTIFIER: { 
      {
        auto id = _builtin_map->find(str);
        if (id != _builtin_map->end()) {
          data.insert(
            data.end(),
            id->second.data.begin(),
            id->second.data.end());
          
          // Jump the instructions that we've used
          // - Minus 1 as we've already accounted for 1 instruction
          //   before coming to this method
          state.instruction_block.bump(id->second.num_instructions-1);
          return;
        }
      }
      return forge::load_symbol(data, str);
    }
    case meta_e::PLUS:
      PARSER_LOAD_AND_EXPECT_GTE_N(machine::ins_id_e::EXEC_ADD, 2);
    case meta_e::SUB:
      PARSER_LOAD_AND_EXPECT_GTE_N(machine::ins_id_e::EXEC_SUB, 2);
    case meta_e::ASTERISK:
      PARSER_LOAD_AND_EXPECT_GTE_N(machine::ins_id_e::EXEC_MUL, 2);
    case meta_e::FORWARD_SLASH:
      PARSER_LOAD_AND_EXPECT_GTE_N(machine::ins_id_e::EXEC_DIV, 2);
    case meta_e::MOD:
      PARSER_LOAD_AND_EXPECT_GTE_N(machine::ins_id_e::EXEC_MOD, 2);
  }
}

} // namespace
