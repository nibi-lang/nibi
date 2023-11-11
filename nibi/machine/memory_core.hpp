#pragma once

#include "machine/env.hpp"
#include "machine/object.hpp"
#include "cppmod/modules.hpp"

#include <string>
#include <memory>

namespace machine {

class memory_core_c {
public:

  memory_core_c() {
    cppmod::populate_env(global_var_env);
  }

  env_c global_var_env;
};

} // namespace
