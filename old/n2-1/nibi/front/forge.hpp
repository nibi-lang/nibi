#pragma once

#include "machine/byte_tools.hpp"
#include "machine/instructions.hpp"
#include "types.hpp"
#include <type_traits>

namespace front {
namespace forge {

struct id_pos_s {
  size_t id{0};
  pos_s pos;
};

struct block_s {
  std::vector<id_pos_s> origins;
  machine::bytes_t data;
};

struct forge_data_s {
  block_s& block;
  pos_s pos;
};

using block_list_t = std::vector<forge::block_s>;

static inline void load_instruction(
  forge_data_s target, const machine::ins_id_e& op) {
  target.block.origins.push_back(
      {target.block.origins.size(),
       target.pos});
  target.block.data.push_back((uint8_t)op);
}

static inline void load_instruction(
  forge_data_s target,
  const machine::ins_id_e& op,
  const machine::bytes_t data) {

  target.block.origins.push_back(
      {target.block.origins.size(),
       target.pos});

  target.block.data.push_back((uint8_t)op);

  auto len_encoded = machine::tools::pack<uint32_t>(data.size());

  target.block.data.insert(
    target.block.data.end(),
    len_encoded.begin(),
    len_encoded.end());

  target.block.data.insert(
    target.block.data.end(),
    data.begin(),
    data.end());
}

template <typename T> void load_raw(forge_data_s target, const T data) {
  machine::ins_id_e op{machine::ins_id_e::PUSH_INT};
  machine::bytes_t encoded = machine::tools::pack<T>(data);
  if (std::is_floating_point<T>::value) {
    op = machine::ins_id_e::PUSH_REAL;
  }
  load_instruction(target, op, encoded);
}

} // namespace
} // namespace
