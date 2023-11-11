#include "cppmod/modules.hpp"

#include "cppmod/io/io.hpp"

namespace cppmod {

void populate_env(machine::env_c& env) {

  env.insert("print", machine::object_c::fn(&io::print));
  env.insert("println", machine::object_c::fn(&io::println));
}

} // namespace
