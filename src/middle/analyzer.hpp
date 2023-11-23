#pragma once 

#include "front/atoms.hpp"
#include "parse_tree.hpp"
#include <optional>

// Take in a parse group of atom lists. Validate, and transform
// them into a parse_tree where they will have the information
// required to transformed by the back end
extern std::optional<parse_list_t> analyze(parse_group_s group);
