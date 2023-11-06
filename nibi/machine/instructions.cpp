#include "instructions.hpp"
#include "byte_tools.hpp"

#include <iostream> // todo: replace with logger

/*
      Instruction layout

      [   id    |      len      |   data ... ]
        1 byte       4 bytes       variable
*/

namespace machine {

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
  bytes_t encoded_len = tools::pack_4(ins_data.size());
  _data.insert(_data.end(), encoded_len.begin(), encoded_len.end());
  _data.insert(_data.end(), ins_data.begin(), ins_data.end());
  return true;
};

std::unique_ptr<instruction_if> instruction_set_builder_c::finalize() {
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


  for (size_t i = 0; i < data.size(); i++) {
    auto& current_byte = data[i];

    if (current_byte >= (uint8_t)ins_id_e::ENUM_BOUNDARY) {
      // Todo: log
      std::cout << "TODO:LOG machine::isbc: Byte is >= boundary " 
                << (int)ins_id_e::ENUM_BOUNDARY 
                << std::endl;
      std::cout << "i = " << i << std::endl;
      std::cout << "size = " << data.size() << std::endl;
      return false;
    }

    ins_id_e op = static_cast<ins_id_e>(current_byte);

    if (current_byte < INS_DATA_BOUNDARY) {
      _instructions.push_back(instruction_s{op, nullptr});
      continue;
    }

    i++;

    bytes_t instruction_data_len_encoded(
      data.begin() + i,
      data.begin() + i + 4);

    std::optional<uint32_t> unpacked_len =
      tools::unpack_4(instruction_data_len_encoded);

    if (!unpacked_len.has_value()) {
      // Todo: log
      std::cout << "TODO:LOG machine::isbc: Failed to pack value"
                << std::endl;
      return false;
    }

    const uint32_t instruction_data_len = *unpacked_len;

    i += 4;

    _instructions.push_back({
      op,
      std::make_unique<bytes_t>(bytes_t(
        data.begin() + i,
        data.begin() + i + instruction_data_len))
      });

    i += instruction_data_len - 1;
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
