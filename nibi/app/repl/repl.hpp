#pragma once

#include "libnibi/interpreter/interpreter.hpp"
#include "libnibi/source.hpp"
#include <optional>
#include <string>

namespace app {

struct repl_config_s {
  std::optional<std::string> prelude{std::nullopt};
};

extern void start_repl(repl_config_s config);

}
