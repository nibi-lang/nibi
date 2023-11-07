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
  target.insert(
    target.end(),
    data.begin(),
    data.end());
}

static inline void load_symbol(
    machine::bytes_t& target,
    const std::string& symbol) {
  machine::bytes_t data(symbol.data(), symbol.data() + symbol.size());
  load_instruction(target, machine::ins_id_e::LOAD_SYMBOL, data);
}

template <typename T> void load_raw(machine::bytes_t& target, const T data) {
  machine::ins_id_e op{machine::ins_id_e::LOAD_INT};
  machine::bytes_t encoded = machine::tools::pack<T>(data);
  if (std::is_floating_point<T>::value) {
    op = machine::ins_id_e::LOAD_REAL;
  }
  load_instruction(target, op, encoded);
}

} // namespace
} // namespace
