#pragma once

#include <functional>
#include <map>
#include <string>
#include "front/atoms.hpp"
#include "vm/instructions.hpp"

using embedded_builder = std::function<void(bytes_t&, atom_list_t& list)>;
using builtin_map = std::map<std::string, embedded_builder>;

extern builtin_map& get_builders();
