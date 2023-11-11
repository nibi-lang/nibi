#include "machine/env.hpp"
#include "machine/object.hpp"
#include "runtime/context.hpp"

namespace cppmod {
namespace io {
extern machine::object_c print(machine::object_list& list, machine::env_c& env);
extern machine::object_c println(machine::object_list& list, machine::env_c& env);
} // namespace
} // namespace 

