#include "cppmod/io/io.hpp"

#include <fmt/format.h>

namespace cppmod {
namespace io {
machine::object_c print(machine::object_list& list, machine::env_c& env) {
  for(auto &&o : list) {
    fmt::print("{}", o.to_string());
  }
  return machine::object_c::integer((int64_t)0);
}

machine::object_c println(machine::object_list& list, machine::env_c& env) {
  print(list, env);
  fmt::print("\n");
  return machine::object_c::integer((int64_t)0);
}

} // namespace
} // namespace 

