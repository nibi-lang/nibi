#include "generator.hpp"
#include "forge.hpp"
#include "machine/instructions.hpp"
#include "machine/object.hpp"
#include "front/builtins.hpp"
#include <stack>
#include <fmt/format.h>
#include <sstream>
#include <iterator>

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
   instruction_set,
   *_tracer.get());

  _ins_receiver.reset_instruction_handling();

}

void generator_c::on_list(atom_list_t list) {
  if (list.empty()) return;
  _lists.push_back(std::move(list));
}

void generator_c::merge_and_register_instructions(block_list_t &blocks) {

  instruction_set.clear();

  std::size_t counter{0};
  for(auto &&block : blocks) {
    // TODO: This will register one position PER BLOCK which MAY contain
    //       multiple instructions. Tracer will have to learn to walk backwards
    //       from error'd instruction to last known block pos
    _tracer->register_instruction(
      counter++, block.origin);

    instruction_set.insert(
      instruction_set.end(),
      block.data.begin(),
      block.data.end());
  }
}

void generator_c::generate() {

  block_list_t blocks;
  auto& top_list = _lists.back();
  auto decomposition_method = _decomp_map.find(top_list[0]->data);
  if (decomposition_method != _decomp_map.end()) {
    auto block_list = decomposition_method->second();
    blocks.insert(
      blocks.end(),
      block_list.begin(),
      block_list.end());
  } else {
    for(auto list = _lists.begin(); list != _lists.end(); list++) {
      auto block_list = decompose_atom_list(*list);
      blocks.insert(
        blocks.end(),
        block_list.begin(),
        block_list.end());

      // Save the results of each list execution
      forge::load_instruction(
        blocks.back().data,
        machine::ins_id_e::SAVE_RESULTS);
    }
  }

  return merge_and_register_instructions(blocks);
}

generator_c::block_list_t generator_c::decompose_atom_list(atom_list_t& list) {

  if (list[0]->atom_type == atom_type_e::NOP) {
    on_error({"First element of list can not be empty", list[0]->pos});
    return {};
  }

  fmt::print("Decomposing list: ");
  print_list(list);

  block_list_t blocks;
  blocks.reserve(list.size());

  auto decomposition_method = _decomp_map.find(list[0]->data);
  if (decomposition_method != _decomp_map.end()) {
    return decomposition_method->second();
  }

  for(std::size_t i = 1; i < list.size(); i++) {
    blocks.push_back(standard_decompose(list[i]));
  }
  
  blocks.push_back(standard_decompose(list[0], true));

  return blocks;
}

generator_c::block_list_t generator_c::decompose_if() {
  block_list_t blocks;

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
    return{};
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
 
  return blocks;
}

generator_c::block_s generator_c::standard_decompose(atom_ptr& atom, bool req_exec) {

  //auto& block = state.instruction_block;
  //auto& data = block.data;

  block_s block{atom->pos, {}};
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
        atom,
        req_exec);
  }
  return block; 
  //_tracer->register_instruction(
  //    state.instruction_block.bump(), atom->pos);
}

#define PARSER_LOAD_AND_EXPECT_GTE_N(op, n) \
 forge::load_instruction( \
     data, machine::ins_id_e::EXPECT_GTE_N_ARGS, machine::tools::pack<uint8_t>(2)); \
  forge::load_instruction(data, op); \
  break;

generator_c::block_s generator_c::decompose_symbol(atom_ptr &atom, bool req_exec) {

  block_s block{atom->pos, {}};
  auto& data = block.data;

  switch (atom->meta) {
    default: {
      fmt::print("{} : {} is not yet implemented - Assuming its an identifier\n\n", (int)atom->meta, atom->data);
      [[fallthrough]];                                                    
    }
    case meta_e::IDENTIFIER: { 
      {
        auto id = _builtin_map->find(atom->data);
        if (id != _builtin_map->end()) {
          data.insert(
            data.end(),
            id->second.data.begin(),
            id->second.data.end());
          return block;
        }
      }
      forge::load_instruction(
        data,
        (req_exec) ? machine::ins_id_e::EXEC_IDENTIFIER :
                      machine::ins_id_e::PUSH_IDENTIFIER,
        machine::tools::pack_string(atom->data));
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
  return block;
}

} // namespace
