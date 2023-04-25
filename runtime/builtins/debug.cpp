#include <iostream>
#include <string>

#include "runtime/builtins/builtins.hpp"
#include "runtime/cell.hpp"
#include "runtime/runtime.hpp"

#include "cpp_macros.hpp"

namespace builtins {

namespace {
void add_debug_info(std::string &result, cell_c &cell_value, cell_c &processed,
                    const char *prefix = "") {

  if (processed.type == cell_type_e::SYMBOL) {
    result += prefix;
    result += "Symbol: ";
    result += processed.to_string();
    result += "\n";
  }

  result += prefix;
  result += "Value: ";
  result += cell_value.to_string();
  result += "\n\n";

  result += "----------------\n";
}
} // namespace

cell_c *builtin_fn_debug_dbg_dbg(cell_list_t &list, env_c &env) {
  if (!global_runtime->is_debug_enabled()) {
    return global_cell_false;
  }
  LIST_ENFORCE_SIZE("dbg", >, 1)
  LIST_ITER_AND_LOAD_SKIP_N(1,
                            { global_runtime->execute_cell(arg, env, true); });
  return global_cell_true;
}

cell_c *builtin_fn_debug_dbg_out(cell_list_t &list, env_c &env) {
  if (!global_runtime->is_debug_enabled()) {
    return global_cell_false;
  }

  LIST_ENFORCE_SIZE("dbg-out", >, 1)
  LIST_ITER_AND_LOAD_SKIP_N(1, { std::cout << arg->to_string() << std::endl; });
  return global_cell_true;
}

cell_c *builtin_fn_debug_dbg_var(cell_list_t &list, env_c &env) {

  if (!global_runtime->is_debug_enabled()) {
    return global_cell_nil;
  }

  LIST_ENFORCE_SIZE("dbg-var", >, 1)

  /*
      Might want to consider doing a source code lookup for the
      resulting output to window in on the source code.
      in the output to show the instruction area.


      Could make a good cause to commonize a source viewer
  */

  auto origin = list.begin();

  auto *dbg_cell = (*origin);

  std::string result;

  result += "\n----------------\n\n";
  result += "`dbg-var` instruction [";
  result += dbg_cell->locator->get_source_name();
  result += ":(";
  result += std::to_string(dbg_cell->locator->get_line());
  result += ",";
  result += std::to_string(dbg_cell->locator->get_column());
  result += ")]\n\nCells given: ";
  result += std::to_string(list.size() - 1);
  result += "\n";

  std::size_t args_count{0};
  LIST_ITER_AND_LOAD_SKIP_N(1, {
    result += "\nArg " + std::to_string(args_count++) + ":\n";

    add_debug_info(result, *arg, *(*it), "\t");
  });

  return global_runtime->get_runtime_memory().allocate(result);
}

} // namespace builtins
