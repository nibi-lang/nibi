#include "instructions.hpp"
#include "byte_tools.hpp"
#include <fmt/format.h>
#include <iostream> // todo: replace with logger

namespace machine {

void print_instruction_data(const uint8_t* bytes, size_t len) {

  uint64_t idx{0};
  uint64_t instruction_count{0};

  struct ins_info_s {
    instruction_view_s* ins{nullptr};
    uint64_t ins_num{0};
    uint64_t byte_idx{0};
  };

  std::vector<ins_info_s> instructions;

  while(idx < len) {

    auto& current_byte = bytes[idx];

    if (current_byte >= (uint8_t)ins_id_e::ENUM_BOUNDARY) {
      fmt::print("ERROR: Detected ENUM_BOUNDARY\n");
      return;
    }

    instruction_view_s* iv =
      (instruction_view_s*)
      ((uint8_t*)(bytes + idx));

    instructions.push_back({
      (instruction_view_s*)((uint8_t*)(bytes + idx)),
      instruction_count,
      idx});

    idx += FIELD_OP_SIZE_BYTES;
    if (current_byte >= INS_DATA_BOUNDARY) {
      idx += FIELD_DATA_LEN_SIZE_BYTES + instructions.back().ins->data_len;
    }

    instruction_count++;
  }

  fmt::print("\n\n\tINSTRUCTION DUMP\n\n");
  for(auto &&x : instructions) {
    fmt::print("op {}\t| ins# {}\t| bci {}\n",
        ins_to_string((ins_id_e)x.ins->op), x.ins_num, x.byte_idx);
  }
  fmt::print("\n\n");
}

const char* ins_to_string(const ins_id_e& id) {
  switch (id) {
  case ins_id_e::NOP: { return "NOP"; }
  case ins_id_e::EXEC_ASSERT: { return "ASSERT"; }
  case ins_id_e::EXEC_IMPORT: { return "IMPORT"; }
  case ins_id_e::EXEC_ADD: { return "ADD"; }
  case ins_id_e::EXEC_SUB: { return "SUB"; }
  case ins_id_e::EXEC_DIV: { return "DIV"; }
  case ins_id_e::EXEC_MUL: { return "MUL"; }
  case ins_id_e::EXEC_MOD: { return "MOD"; }
  case ins_id_e::EXEC_EQ: { return "EQ"; }
  case ins_id_e::EXEC_NE: { return "NE"; }
  case ins_id_e::EXEC_LT: { return "LT"; }
  case ins_id_e::EXEC_GT: { return "GT"; }
  case ins_id_e::EXEC_ASSIGN: { return "ASSIGN"; }
  case ins_id_e::EXEC_REASSIGN: { return "REASSIGN"; }
  case ins_id_e::EXEC_DBG: { return "DBG"; }
  case ins_id_e::PUSH_RESULT: { return "P_RESULT"; }
  case ins_id_e::SAVE_RESULTS: { return "S_RESULTS"; }      
  case ins_id_e::PUSH_STRING: { return "P_STRING"; }
  case ins_id_e::PUSH_INT: { return "P_INT"; }
  case ins_id_e::PUSH_REAL: { return "P_REAL"; }
  case ins_id_e::PUSH_IDENTIFIER: { return "P_ID"; }
  case ins_id_e::EXEC_IDENTIFIER: { return "E_ID"; }
  case ins_id_e::EXPECT_N_ARGS: { return "EX_N_ARG"; }
  case ins_id_e::EXPECT_GTE_N_ARGS: { return "EX_GTE_N_ARGS"; }
  case ins_id_e::EXPECT_OBJECT_TYPE: { return "EX_OBJ_TYPE"; }
  case ins_id_e::ENUM_BOUNDARY: { return "ENUM_BOUNDARY"; }
  }
  return "UNKNOWN";
}

/*


bool instruction_set_builder_c::encode_instruction(const ins_id_e id) {
  if ((uint8_t)id >= INS_DATA_BOUNDARY) {
    return false;
  }
  _data.push_back((uint8_t)id);
  return true;
}

bool instruction_set_builder_c::encode_instruction(
  const ins_id_e id, const bytes_t& ins_data) {
  if ((uint8_t)id < INS_DATA_BOUNDARY) {
    return false;
  }
  _data.push_back((uint8_t)id);
  bytes_t encoded_len = tools::pack<uint32_t>(ins_data.size());
  _data.insert(_data.end(), encoded_len.begin(), encoded_len.end());
  _data.insert(_data.end(), ins_data.begin(), ins_data.end());
  return true;
};

std::unique_ptr<instruction_if>
instruction_set_builder_c::get_instruction_interface() {
  _data.shrink_to_fit();
  
  auto final_instructions =
    std::make_unique<instruction_set_c>();

  if (!final_instructions->populate(_data)) {
    return nullptr;
  }

  _data.clear();

  return std::move(final_instructions);
}

bool instruction_set_builder_c::instruction_set_c::populate(
  const bytes_t& data) {

  _data.clear();

  size_t idx{0};
  while(idx < data.size()) {
    auto& current_byte = data[idx];

    if (current_byte >= (uint8_t)ins_id_e::ENUM_BOUNDARY) {
      // Todo: log
      std::cout << "TODO:LOG machine::isbc: Byte is >= boundary " 
                << (int)ins_id_e::ENUM_BOUNDARY 
                << std::endl;
      std::cout << "i = " << idx << std::endl;
      std::cout << "size = " << data.size() << std::endl;
      return false;
    }

    instruction_view_s* iv =
      (instruction_view_s*)((uint8_t*)(data.data() + idx));

    if (current_byte < INS_DATA_BOUNDARY) {
      _instructions.push_back(instruction_s{(ins_id_e)iv->op, nullptr});
      idx += FIELD_OP_SIZE_BYTES;
      continue;
    }

    idx += FIELD_OP_SIZE_BYTES + FIELD_DATA_LEN_SIZE_BYTES;

    if (idx + iv->data_len >= data.size() ) {
      // Todo: Log here
      std::cout << "Error: Insufficent data to construct instruction" << std::endl;
      return false;
    }

    _instructions.push_back({
      (ins_id_e)iv->op,
      std::make_unique<bytes_t>(bytes_t(
        iv->data,
        iv->data + iv->data_len))
      });

    idx += iv->data_len;
  }
  return true;
}

const instruction_s& instruction_set_builder_c::instruction_set_c::get(
  const size_t index, bool &okay) const {
  if (index >= _instructions.size()) {
    okay = false;
    return _nop;
  }
  okay = true;
  return _instructions[index];
}

const size_t instruction_set_builder_c::instruction_set_c::size() const {
  return _instructions.size();
}

*/

} // namespace