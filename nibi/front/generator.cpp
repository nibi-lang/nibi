#include "generator.hpp"
#include "forge.hpp"
#include "machine/instructions.hpp"
#include "machine/object.hpp"
#include <stack>
#include <fmt/format.h>
#include <sstream>
#include <iterator>

namespace front {

namespace {
  inline static void add_instruction(
      forge::forge_data_s bs,
      const machine::ins_id_e& id) {
    forge::load_instruction(bs, id);
  }
  inline static void add_instruction(
      forge::forge_data_s bs,
      const machine::ins_id_e& id,
      const machine::bytes_t& data) {
    forge::load_instruction(bs, id, data);
  }
  inline static void expect_type(
      forge::forge_data_s bs,
      const machine::data_type_e& type) {
      add_instruction(
        bs,
        machine::ins_id_e::EXPECT_OBJECT_TYPE,
        machine::tools::pack<uint8_t>(
          (uint8_t)type));
  }
  inline static void expect_none(forge::forge_data_s bs) {
    return expect_type(bs, machine::data_type_e::NONE);
  }
  inline static void expect_boolean(forge::forge_data_s bs) {
    return expect_type(bs, machine::data_type_e::BOOLEAN);
  }
  inline static void expect_integer(forge::forge_data_s bs) {
    return expect_type(bs, machine::data_type_e::INTEGER);
  }
  inline static void expect_real(forge::forge_data_s bs) {
    return expect_type(bs, machine::data_type_e::REAL);
  }
  inline static void expect_string(forge::forge_data_s bs) {
    return expect_type(bs, machine::data_type_e::STRING);
  }
  inline static void expect_identifier(forge::forge_data_s bs) {
    return expect_type(bs, machine::data_type_e::IDENTIFIER);
  }
  inline static void expect_bytes(forge::forge_data_s bs) {
    return expect_type(bs, machine::data_type_e::BYTES);
  }
  inline static void expect_ref(forge::forge_data_s bs) {
    return expect_type(bs, machine::data_type_e::REF);
  }
  inline static void expect_error(forge::forge_data_s bs) {
    return expect_type(bs, machine::data_type_e::ERROR);
  }
  inline static void expect_exactly_n_args(
    forge::forge_data_s bs, const uint8_t& n) {
    add_instruction(
      bs,
      machine::ins_id_e::EXPECT_N_ARGS,
      machine::tools::pack<uint8_t>(n));
  }
  inline static void expect_at_least_n_args(
    forge::forge_data_s bs, const uint8_t& n) {
    add_instruction(
      bs,
      machine::ins_id_e::EXPECT_GTE_N_ARGS,
      machine::tools::pack<uint8_t>(n));
  }
} // namespace

generator_c::generator_c(
  tracer_ptr tracer,
  machine::instruction_receiver_if& ins_receiver)
  : _tracer(tracer),
    _ins_receiver(ins_receiver) {
  _decomp_map = {
  { "let", std::bind(&generator_c::decompose_let, this, std::placeholders::_1)},
   { "set", std::bind(&generator_c::decompose_set, this, std::placeholders::_1)},
   { "use", std::bind(&generator_c::decompose_use, this, std::placeholders::_1)},
   { "dbg", std::bind(&generator_c::decompose_dbg, this, std::placeholders::_1)},
   { "if", std::bind(&generator_c::decompose_if, this, std::placeholders::_1)},
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
  instruction_set = {};
  _lists.clear();
}

void generator_c::on_list(atom_list_t list) {
  if (list.empty()) return;

  _lists.push_back(std::move(list));
}

void generator_c::merge_and_register_instructions(forge::block_list_t &blocks) {

  std::size_t offset{0};
  for(auto &&block : blocks) {

    // For tracing errors we store the instruction # along with
    // the file position the instruction was generated from,
    // we _could_ disable this in release.
    for(auto&& o : block.origins) {
      _tracer->register_instruction(
        o.id + offset, o.pos);
    }
    offset += block.origins.size();

    // Merge the block with the instruction set we will send
    // to the engine
    instruction_set.insert(
      instruction_set.end(),
      block.data.begin(),
      block.data.end());
  }
}

void generator_c::generate() {

  forge::block_list_t blocks;
    for(auto list = _lists.begin(); list != _lists.end(); list++) {
      auto block_list = decompose_atom_list(*list);
      blocks.insert(
        blocks.end(),
        block_list.begin(),
        block_list.end());

      add_instruction({blocks.back(), list->back()->pos}, machine::ins_id_e::SAVE_RESULTS);
    }
    /*
  auto& top_list = _lists.back();
  auto decomposition_method = _decomp_map.find(top_list[0]->data);
  if (decomposition_method != _decomp_map.end()) {
    auto block_list = decomposition_method->second(top_list);
    blocks.insert(
      blocks.end(),
      block_list.begin(),
      block_list.end());

      add_instruction({blocks.back(), top_list.back()->pos}, machine::ins_id_e::SAVE_RESULTS);

  } else {
    for(auto list = _lists.begin(); list != _lists.end(); list++) {
      auto block_list = decompose_atom_list(*list);
      blocks.insert(
        blocks.end(),
        block_list.begin(),
        block_list.end());

      add_instruction({blocks.back(), list->back()->pos}, machine::ins_id_e::SAVE_RESULTS);
    }
  }
*/
  return merge_and_register_instructions(blocks);
}

#define DECOMPOSE_PARAMS\
  for(size_t i = 1; i < list.size(); ++i) {\
    standard_decompose(blocks, list[i]);\
  }

forge::block_list_t generator_c::decompose_atom_list(atom_list_t& list) {

  if (list[0]->atom_type == atom_type_e::NOP) {
    on_error({"First element of list can not be empty", list[0]->pos});
    return {};
  }

  fmt::print("Decomposing list: ");
  print_list(list);

  forge::block_list_t blocks;
  blocks.reserve(list.size());

  auto decomposition_method = _decomp_map.find(list[0]->data);
  if (decomposition_method != _decomp_map.end()) {
    return decomposition_method->second(list);
  }

  DECOMPOSE_PARAMS
  
  standard_decompose(blocks, list[0], true);
  return blocks;
}

void generator_c::standard_decompose(forge::block_list_t& blocks, atom_ptr& atom, bool req_exec) {

  forge::block_s block{{}, {}};
  auto& data = block.data;

  switch(atom->atom_type) {
    case atom_type_e::UNDEFINED: {
      // TODO: Burn everything down
      break;
    }
    case atom_type_e::NOP: {
      forge::load_instruction(
       { block, atom->pos },
        machine::ins_id_e::NOP);
      break;
    }
    case atom_type_e::INTEGER:
      forge::load_raw<int64_t>(
       { block, atom->pos },
        std::stoull(atom->data.c_str()));
      break;
    case atom_type_e::REAL:
      forge::load_raw<double>(
       { block, atom->pos },
        std::stod(atom->data.c_str()));
      break;
    case atom_type_e::STRING:
      forge::load_instruction(
       { block, atom->pos },
        machine::ins_id_e::PUSH_STRING,
        machine::tools::pack_string(atom->data));
      break;
    case atom_type_e::LOAD_ARG:
      forge::load_instruction(
       { block, atom->pos },
        machine::ins_id_e::PUSH_RESULT);
      break;
    case atom_type_e::SYMBOL:   // Symbols log their own instruction 
      return decompose_symbol(  // so we return here
        blocks,
        atom,
        req_exec);
  }
  blocks.push_back(block);
}

void generator_c::decompose_symbol(forge::block_list_t& blocks, atom_ptr &atom, bool req_exec) {

  forge::block_s block{{}, {}};
  forge::forge_data_s data{block, atom->pos};

  switch (atom->meta) {
    default: {
      fmt::print("{} : {} is not yet implemented - Assuming its an identifier\n\n", (int)atom->meta, atom->data);
      [[fallthrough]];                                                    
    }
    case meta_e::IDENTIFIER: { 
      forge::load_instruction(
       { block, atom->pos },
        (req_exec) ? machine::ins_id_e::EXEC_IDENTIFIER :
                      machine::ins_id_e::PUSH_IDENTIFIER,
        machine::tools::pack_string(atom->data));
      break;
    }
    case meta_e::PLUS:
      expect_at_least_n_args(data, 2);
      add_instruction(data, machine::ins_id_e::EXEC_ADD);
      break;
    case meta_e::SUB:
      expect_at_least_n_args(data, 2);
      add_instruction(data, machine::ins_id_e::EXEC_SUB);
      break;
    case meta_e::ASTERISK:
      expect_at_least_n_args(data, 2);
      add_instruction(data, machine::ins_id_e::EXEC_MUL);
      break;
    case meta_e::FORWARD_SLASH:
      expect_at_least_n_args(data, 2);
      add_instruction(data, machine::ins_id_e::EXEC_DIV);
      break;
    case meta_e::MOD:
      expect_at_least_n_args(data, 2);
      add_instruction(data, machine::ins_id_e::EXEC_MOD);
      break;
  }
  blocks.push_back(block);
}

// ---------------------------------- 
//
//      BUILTINS
//
// ---------------------------------- 

forge::block_list_t generator_c::decompose_let(atom_list_t& list) {

  forge::block_list_t blocks;

  DECOMPOSE_PARAMS

  forge::block_s code;
  forge::forge_data_s data{code, list[0]->pos};
  expect_exactly_n_args(data, 2);
  expect_identifier(data);
  add_instruction(
    data,
    machine::ins_id_e::EXEC_ASSIGN);

  blocks.push_back(code);

  // we may want to place the value back on the queue as
  // a ref to the obejct
  return blocks;
}
forge::block_list_t generator_c::decompose_set(atom_list_t& list) {
  forge::block_list_t blocks;

  DECOMPOSE_PARAMS

  forge::block_s code;
  forge::forge_data_s data{code, list[0]->pos};
  expect_exactly_n_args(data, 2);
  expect_identifier(data);
  add_instruction(
    data,
    machine::ins_id_e::EXEC_REASSIGN);

  blocks.push_back(code);

  // we may want to place the value back on the queue as
  // a ref to the obejct
  return blocks;
}
forge::block_list_t generator_c::decompose_use(atom_list_t& list) {
  forge::block_list_t blocks;

  DECOMPOSE_PARAMS

  forge::block_s code;
  forge::forge_data_s data{code, list[0]->pos};
  expect_at_least_n_args(data, 1);
  add_instruction(
    data,
    machine::ins_id_e::EXEC_IMPORT);
  blocks.push_back(code);
  return blocks;
}
forge::block_list_t generator_c::decompose_dbg(atom_list_t& list) {
  forge::block_list_t blocks;

  DECOMPOSE_PARAMS

  forge::block_s code;
  forge::forge_data_s data{code, list[0]->pos};
  expect_at_least_n_args(data, 1);
  add_instruction(
    data,
    machine::ins_id_e::EXEC_DBG);
  blocks.push_back(code);
  return blocks;
}

forge::block_list_t generator_c::decompose_if(atom_list_t& list) {
  forge::block_list_t blocks;

  fmt::print("Need to decompose an if-stmt\n");
/*
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
*/
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

} // namespace
