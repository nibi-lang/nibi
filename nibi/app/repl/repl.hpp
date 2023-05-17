#pragma once

#include "libnibi/interpreter/interpreter.hpp"
#include "libnibi/source.hpp"

namespace app {

extern void start_repl(nibi::interpreter_c &interpreter,
                       nibi::source_manager_c &source_manager);

}