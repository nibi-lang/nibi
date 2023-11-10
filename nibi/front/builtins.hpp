#pragma once

#include "front/forge.hpp"
#include <map>
#include <string>

namespace front {
namespace builtins {

struct builtin_s {
  size_t num_instructions{0};
  machine::bytes_t data;
};

using builtin_map_t = std::map<std::string, builtin_s>;

// Get the builtin bytecode instruction groups
extern builtin_map_t& get_builtins();

} // namespace
} // namespace
