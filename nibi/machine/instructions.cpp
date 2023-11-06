#include "instructions.hpp"
#include "byte_tools.hpp"

#include <iostream> // todo: replace with logger

namespace machine {

namespace {
  static constexpr uint8_t FIELD_OP_SIZE_BYTES = 1;
  static constexpr uint8_t FIELD_DATA_LEN_SIZE_BYTES = 4;

}

instruction_set_builder_c::instruction_set_builder_c() {
  _data.reserve(MIBI_BYTE * 4);
}

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

} // namespace
