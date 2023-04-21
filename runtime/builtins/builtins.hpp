#pragma once

#include "runtime/env.hpp"
#include "runtime/cell.hpp"

namespace builtins {

extern cell_c* builtin_fn_arithmetic_add(cell_list_t args, env_c &env);

}