#include "parser.hpp"
#include "forge.hpp"
#include "machine/instructions.hpp"
#include "machine/object.hpp"
#include "front/builtins.hpp"
#include <stack>
#include <fmt/format.h>
#include <sstream>

namespace front {

parser_c::parser_c(
  tracer_ptr tracer,
  machine::instruction_receiver_if& ins_receiver)
  : _tracer(tracer),
    _ins_receiver(ins_receiver) {
  _builtin_map = &builtins::get_builtins();
}

void parser_c::on_error(error_s error) {
  fmt::print("Parser received an error report\n");;
  _tracer->on_error(error);
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

  _ins_receiver.reset_instruction_handling();

  state.reset(); 
}

void parser_c::on_list(atom_list_t list) {

  if (list.empty()) return;

  auto& block = state.instruction_block;
  auto& data = block.data;

  state.current_list = &list;

  do {
    decompose(state.get_current());
    state.next();
  } while (state.has_next());

  decompose(list[0], true);

  state.list_depth++;

  _tracer->register_instruction(
      state.instruction_block.bump(), list.back()->pos);
  forge::load_instruction(data, machine::ins_id_e::SAVE_RESULTS);
}

void parser_c::decompose(atom_ptr& atom, bool req_exec) {

  auto& block = state.instruction_block;
  auto& data = block.data;

  switch(atom->atom_type) {
    case atom_type_e::UNDEFINED: {
      // TODO: Burn everything down
      break;
    }
    case atom_type_e::INTEGER:
      forge::load_raw<int64_t>(
        data,
        std::stoull(atom->data.c_str()));
      break;
    case atom_type_e::REAL:
      forge::load_raw<double>(
        data,
        std::stod(atom->data.c_str()));
      break;
    case atom_type_e::STRING:
      forge::load_instruction(
        data,
        machine::ins_id_e::PUSH_STRING,
        machine::tools::pack_string(atom->data));
      break;
    case atom_type_e::LOAD_ARG:
      forge::load_instruction(
        data,
        machine::ins_id_e::PUSH_RESULT);
      break;
    case atom_type_e::SYMBOL:   // Symbols log their own instruction 
      return decompose_symbol(  // so we return here
        atom->meta,
        atom->data,
        atom->pos,
        req_exec);
  }
  
  _tracer->register_instruction(
      state.instruction_block.bump(), atom->pos);
}

#define PARSER_LOAD_AND_EXPECT_GTE_N(op, n) \
 forge::load_instruction( \
     data, machine::ins_id_e::EXPECT_GTE_N_ARGS, machine::tools::pack<uint8_t>(2)); \
  _tracer->register_instruction(\
      state.instruction_block.bump(), pos); \
  forge::load_instruction(data, op); \
  break;

void parser_c::decompose_symbol(
  const meta_e& meta, const std::string& str, const pos_s& pos, bool req_exec) {

  auto& block = state.instruction_block;
  auto& data = block.data;

  switch (meta) {
    default: {
      fmt::print("{} : {} is not yet implemented - Assuming its an identifier\n\n", (int)meta, str);
      [[fallthrough]];                                                    
    }
    case meta_e::ACCESSOR: {
      return build_accessor(str, req_exec);
    }
    case meta_e::IDENTIFIER: { 
      {
        auto id = _builtin_map->find(str);
        if (id != _builtin_map->end()) {
          data.insert(
            data.end(),
            id->second.data.begin(),
            id->second.data.end());
 
          // Add position for each instruction generated
          for(size_t i = 0; i < id->second.num_instructions; i++) {
            _tracer->register_instruction(
                state.instruction_block.bump(), pos);
          }
          return;
        }
      }
      forge::load_instruction(
        data,
        ((req_exec) ? machine::ins_id_e::EXEC_IDENTIFIER :
                      machine::ins_id_e::PUSH_IDENTIFIER)),
        machine::tools::pack_string(str);
      break;
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
  _tracer->register_instruction(
      state.instruction_block.bump(), pos);
}

void parser_c::build_accessor(const std::string& str, bool req_exec) {

  std::stack<std::string> fields; 

  std::vector<std::string> accessors;
  accessors.reserve(10);

  std::string target;
  std::stringstream source(str);

  while (std::getline(source, target, '.')) {
    accessors.push_back(target);
  }

  for(auto i = 0; i < accessors.size()-1; i++) {
    fmt::print("LOAD ENV {} \n", accessors[i]);
    fields.push(accessors[i]);
  }

  fmt::print("Call ID: {}\n", accessors.back());

  while(!fields.empty()) {
    
    fmt::print("UNLOAD ENV {}\n", fields.top());
    fields.pop();
  }
}

} // namespace
