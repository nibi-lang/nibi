#pragma once

#include <cstdint>
#include <string>
#include "runtime/context.hpp"

namespace front {
namespace intake {

struct settings_s {
  runtime::context_c ctx;
};

extern uint8_t repl(
  settings_s& settings_s);

extern uint8_t file(
  settings_s& settings_s,
  const std::string&); 

extern uint8_t dir(
  settings_s& settings_s,
  const std::string&);

} // namespace
} // namespace
