#include "cppmod/modules.hpp"

#include "cppmod/io/io.hpp"

namespace cppmod {

void populate_env(machine::env_c& env) {

  /*

      I want actual `.` syntax for loaded environments.
      We need to make an "env" object that can support this.
      The byte code can take the following:

          io.print

      and turn it into 

      EXEC_LOAD_ENV <io>
      EXEC_LOAD_IDENTIFIER (As it does now)
      EXEC_UNLOAD_ENV



  */


  env.insert("print", machine::object_c::fn(&io::print));
  env.insert("println", machine::object_c::fn(&io::println));
}

} // namespace
