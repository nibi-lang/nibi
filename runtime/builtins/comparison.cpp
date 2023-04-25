#include <iostream>

#include "arithmetic_helpers.hpp"
#include "runtime/builtins/builtins.hpp"
#include "runtime/builtins/cpp_macros.hpp"
#include "runtime/cell.hpp"
#include "runtime/runtime.hpp"

namespace builtins {


cell_c* builtin_fn_comparison_eq(cell_list_t &list, env_c &env) { return global_cell_false; }
cell_c* builtin_fn_comparison_neq(cell_list_t &list, env_c &env) { return global_cell_false; }
cell_c* builtin_fn_comparison_lt(cell_list_t &list, env_c &env) { return global_cell_false; }
cell_c* builtin_fn_comparison_gt(cell_list_t &list, env_c &env) { return global_cell_false; }
cell_c* builtin_fn_comparison_lte(cell_list_t &list, env_c &env) { return global_cell_false; }
cell_c* builtin_fn_comparison_gte(cell_list_t &list, env_c &env) { return global_cell_false; }

}