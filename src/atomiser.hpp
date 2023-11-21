#pragma once

#include "types.hpp"
#include "atoms.hpp"

#include <set>
#pragma once

#include "types.hpp"

#include <optional>
#include <string>

extern std::optional<parse_group_s> atomise_file(const std::string& file);

extern int repl();

