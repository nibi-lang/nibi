#pragma once

#include <cstdint>
#include <string>

namespace front {
namespace intake {

struct settings_s {
  //  - Input arguments
  //  - User flags, etc
  //
  //
  // engine_c::settings_s
};

extern uint8_t repl(
  const settings_s& settings_s);

extern uint8_t file(
  const settings_s& settings_s,
  const std::string&); 

extern uint8_t dir(
  const settings_s& settings_s,
  const std::string&);

} // namespace
} // namespace
