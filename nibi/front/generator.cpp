#include "generator.hpp"
#include "forge.hpp"
#include "machine/instructions.hpp"
#include "machine/object.hpp"
#include "front/builtins.hpp"
#include <stack>
#include <fmt/format.h>
#include <sstream>

namespace front {

generator_c::generator_c(
  tracer_ptr tracer,
  machine::instruction_receiver_if& ins_receiver)
  : _tracer(tracer),
    _ins_receiver(ins_receiver) {
  _builtin_map = &builtins::get_builtins();
  _decomp_map = {
    { "if", std::bind(&generator_c::decompose_if, this)},
  };
}

void generator_c::on_error(error_s error) {
  fmt::print("Parser received an error report\n");;
  _tracer->on_error(error);
}

void generator_c::on_top_list_complete() {
  // Note: 
  // Engine _must_ be instructed to copy any instruction over
  // that may be referenced later

  // Tracers are stored in an active runtime context. As long as the context is alive,
  // and the tracer was properly registered with the context,
  // the reference to the shared value will live on. We _could_ update the machine
  // to take a tracer, but for the sake of loose coupling, we are using the interface
  // provided by machine that the tracer utilizes

  if (_lists.empty()) {
    return;
  }

  generate();

  _ins_receiver.handle_instructions(
    state.instruction_block.data,
    *_tracer.get());

  _ins_receiver.reset_instruction_handling();

  state.reset(); 
}


void generator_c::on_list(atom_list_t list) {
  if (list.empty()) return;
  _lists.push_back(std::move(list));
}

void generator_c::generate() {

  auto& top_list = _lists.back();

  fmt::print("_lists is of size: {}\n", _lists.size());
  fmt::print("Top list is of size: {}\n", top_list.size());

  fmt::print("First item : {}\n",
    top_list[0]->data);

  auto decomposition_method = _decomp_map.find(top_list[0]->data);
  if (decomposition_method != _decomp_map.end()) {
    return decomposition_method->second();
  }

  for(auto &&list : _lists) {
    auto& block = state.instruction_block;
    auto& data = block.data;

    state.current_list = &list;

    for(state.current_list_idx = 1;
        state.current_list_idx < state.current_list->size();
        state.current_list_idx++) {
      standard_decompose(state.get_current());
    }

    standard_decompose(list[0], true);
    // TODO: cant let first item be NOP

    state.list_depth++;

    _tracer->register_instruction(
        state.instruction_block.bump(), list.back()->pos);
    forge::load_instruction(data, machine::ins_id_e::SAVE_RESULTS);
  }
}

void generator_c::decompose_if() {

  fmt::print("Need to decompose an if-stmt\n");

  pos_s pos = _lists.back()[0]->pos;
  _lists.pop_back();

  atom_list_t* condition{nullptr};
  atom_list_t* true_body{nullptr};
  atom_list_t* false_body{nullptr};

  if (_lists.size() == 2) {
    condition = &_lists[0];
    true_body = &_lists[1];
  } else if (_lists.size() == 3) {
    condition = &_lists[0];
    true_body = &_lists[1];
    false_body = &_lists[2];
  } else {
    on_error(
      { fmt::format("'if' requires 2 or 3 parameters, {} were given", _lists.size()),
        pos});
    return;
  }

  fmt::print("Condition: ");
  print_list(*condition);

  if (true_body) {
    fmt::print("TRUE body: ");
    print_list(*true_body);
  }

  if (false_body) {
    fmt::print("FALSE body: ");
    print_list(*false_body);
  }

  // decompose condition into list
  // decompose true into list. get size.
  // decompose false into list. get size
  

  // Add condition into main instruction
  // create instruction for EXEC_CONDITIONAL_JUMP
  //    if condition true, don't jump, then EXEC_JUMP over false size
  //    if condition false, jump `size` instruction bytes to execute false
  
 // TODO : Add in engine a getter for queue. if queue empty, return object_c::interger(0)


}

void generator_c::standard_decompose(atom_ptr& atom, bool req_exec) {

  auto& block = state.instruction_block;
  auto& data = block.data;

  switch(atom->atom_type) {
    case atom_type_e::UNDEFINED: {
      // TODO: Burn everything down
      break;
    }
    case atom_type_e::NOP: {
      forge::load_instruction(
        data,
        machine::ins_id_e::NOP);
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

void generator_c::decompose_symbol(
  const meta_e& meta, const std::string& str, const pos_s& pos, bool req_exec) {

  auto& block = state.instruction_block;
  auto& data = block.data;

  switch (meta) {
    default: {
      fmt::print("{} : {} is not yet implemented - Assuming its an identifier\n\n", (int)meta, str);
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
        (req_exec) ? machine::ins_id_e::EXEC_IDENTIFIER :
                      machine::ins_id_e::PUSH_IDENTIFIER,
        machine::tools::pack_string(str));
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

} // namespace
