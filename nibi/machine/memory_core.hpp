#pragma once

#include "machine/env.hpp"
#include "machine/object.hpp"

#include <string>
#include <memory>

namespace machine {

class memory_core_c {
public:
  env_c global_var_env;
};

} // namespace
