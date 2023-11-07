#include "parser.hpp"
#include "forge.hpp"
#include "machine/instructions.hpp"

#include <iostream>

namespace front {

namespace {
  struct builtin_s {
    machine::bytes_t instructions;
  };
}

static std::unique_ptr<
  std::map<std::string, builtin_s>> builtins{nullptr};



static inline void populate_builtins() {
  
  if (builtins) { return; }

  builtins = std::make_unique<
    std::map<std::string, builtin_s>>();

  builtins->insert(std::make_pair("let",
    builtin_s{
      machine::bytes_t{ (uint8_t)machine::ins_id_e::EXEC_ASSIGN }
      }
    )
  );


  // TODO: Populate builtin instruction's byte code
  //
  //    - Validate argument count, checkable types (VAR, STR, INT, REAL)
  //          ex:  LOAD_VAR, ASSERT_TYPE, Etc
  //          ^ -- Don't attempt to DO this here, generate the CODE to
  //               do this upon execution.
  //
  //    More for functions:
  //      INIT_STATIC_INS (for defn)
  //      COLL_STATIC_INS (For defn
  //
  //          - All "static" instructions should assume that they
  //            _may_ be optimized by the engine
  //
  //      ENTER_SCOPE (with marker data for named/ tagged scopes),  
  //      LEAVE_SCOPE (with marker containing the tag to leave),
  //
  //      
  //      
  //
  //
}


parser_c::parser_c(
  tracer_ptr tracer,
  machine::instruction_receiver_if& ins_receiver)
  : _tracer(tracer),
    _ins_receiver(ins_receiver) {
  populate_builtins();
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
  forge::load_instruction(data, machine::ins_id_e::NEW_PROC_FRAME);

  for(auto &&atom : list) {
    decompose(atom);
    state.list_depth++;
  }

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
        atom->data);
  }
}

void parser_c::decompose_symbol(
  const meta_e& meta, const std::string& str) {

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
        auto id = builtins->find(str);
        if (id != builtins->end()) {
          data.insert(
            data.end(),
            id->second.instructions.begin(),
            id->second.instructions.end());
          return;
        }
      }
      return forge::load_symbol(data, str);
    }
    case meta_e::PLUS:
      return forge::load_instruction(data, machine::ins_id_e::EXEC_ADD);
    case meta_e::SUB:
      return forge::load_instruction(data, machine::ins_id_e::EXEC_SUB);
    case meta_e::ASTERISK:
      return forge::load_instruction(data, machine::ins_id_e::EXEC_MUL);
    case meta_e::FORWARD_SLASH:
      return forge::load_instruction(data, machine::ins_id_e::EXEC_DIV);
    case meta_e::MOD:
      return forge::load_instruction(data, machine::ins_id_e::EXEC_MOD);
  }
}

} // namespace
