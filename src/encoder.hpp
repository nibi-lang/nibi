#pragma once

#include "atoms.hpp"
#include "machine.hpp"
#include <functional>

using generator_error_cb = std::function<void(
  const std::string& error,
  const file_position_s& pos)>;

extern bytes_t generate_instructions(
  atom_list_t& list,
  generator_error_cb);
  
