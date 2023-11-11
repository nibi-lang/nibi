#pragma once

#include "machine/byte_tools.hpp"
#include "machine/instructions.hpp"
#include <type_traits>

namespace front {
namespace forge {

static inline void load_instruction(
  machine::bytes_t& target, const machine::ins_id_e& op) {
  target.push_back((uint8_t)op);
}

static inline void load_instruction(
  machine::bytes_t& target,
  const machine::ins_id_e& op,
  const machine::bytes_t& data) {
  target.push_back((uint8_t)op);

  auto len_encoded = machine::tools::pack<uint32_t>(data.size());

  target.insert(
    target.end(),
    len_encoded.begin(),
    len_encoded.end());

  target.insert(
    target.end(),
    data.begin(),
    data.end());
}

template <typename T> void load_raw(machine::bytes_t& target, const T data) {
  machine::ins_id_e op{machine::ins_id_e::PUSH_INT};
  machine::bytes_t encoded = machine::tools::pack<T>(data);
  if (std::is_floating_point<T>::value) {
    op = machine::ins_id_e::PUSH_REAL;
  }
  load_instruction(target, op, encoded);
}

} // namespace
} // namespace
