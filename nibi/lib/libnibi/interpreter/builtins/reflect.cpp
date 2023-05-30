#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "libnibi/cell.hpp"

#include "macros.hpp"

namespace nibi {

namespace builtins {

namespace {
inline nibi::cell_ptr type(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                           nibi::env_c &env) {
  auto resolved = ci.execute_cell(list[1], env);
  switch (resolved->type) {
  case nibi::cell_type_e::ABERRANT:
    return nibi::allocate_cell("aberrant");
  case nibi::cell_type_e::NIL:
    return nibi::allocate_cell("nil");
  case nibi::cell_type_e::INTEGER:
    return nibi::allocate_cell("int");
  case nibi::cell_type_e::DOUBLE:
    return nibi::allocate_cell("float");
  case nibi::cell_type_e::STRING:
    return nibi::allocate_cell("string");
  case nibi::cell_type_e::LIST: {
    auto list_info = resolved->as_list_info();
    switch (list_info.type) {
    case nibi::list_types_e::DATA:
      return nibi::allocate_cell("list:data");
    case nibi::list_types_e::ACCESS:
      return nibi::allocate_cell("list:access");
    case nibi::list_types_e::INSTRUCTION:
      return nibi::allocate_cell("list:instruction");
    default:
      return nibi::allocate_cell("list");
    }
  }
  case nibi::cell_type_e::FUNCTION:
    return nibi::allocate_cell("function");
  case nibi::cell_type_e::ENVIRONMENT:
    return nibi::allocate_cell("environment");
  case nibi::cell_type_e::SYMBOL:
    return nibi::allocate_cell("symbol");
  default:
    return nibi::allocate_cell("unknown");
  }
}

} // namespace

cell_ptr builtin_fn_reflect_type(interpreter_c &ci, cell_list_t &list,
                                 env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("type", ==, 2)
  return type(ci, list, env);
}

} // namespace builtins

} // namespace nibi
