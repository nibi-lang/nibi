#pragma once

#include "machine/env.hpp"
#include "machine/object.hpp"

namespace cppmod {

// Populate an enviornment with the 
// builtin cpp methods
extern void populate_env(machine::env_c& env);

} // namespace 
