#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/keywords.hpp"
#include "libnibi/type_names.hpp"
#include "macros.hpp"

namespace nibi {

namespace builtins {

namespace {
inline nibi::cell_ptr type(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                           nibi::env_c &env) {
  auto resolved = ci.process_cell(list[1], env);
  switch (resolved->type) {
  case nibi::cell_type_e::ABERRANT:
    return nibi::allocate_cell(nibi::types::ABERRANT);
  case nibi::cell_type_e::NIL:
    return nibi::allocate_cell(nibi::types::NIL);
  case nibi::cell_type_e::INTEGER:
    return nibi::allocate_cell(nibi::types::INT);
  case nibi::cell_type_e::DOUBLE:
    return nibi::allocate_cell(nibi::types::FLOAT);
  case nibi::cell_type_e::STRING:
    return nibi::allocate_cell(nibi::types::STRING);
  case nibi::cell_type_e::LIST: {
    auto list_info = resolved->as_list_info();
    switch (list_info.type) {
    case nibi::list_types_e::DATA:
      return nibi::allocate_cell(nibi::types::LIST_DATA);
    case nibi::list_types_e::ACCESS:
      return nibi::allocate_cell(nibi::types::LIST_ACCESS);
    case nibi::list_types_e::INSTRUCTION:
      return nibi::allocate_cell(nibi::types::LIST_INSTRUCTION);
    default:
      return nibi::allocate_cell(nibi::types::LIST);
    }
  }
  case nibi::cell_type_e::FUNCTION:
    return nibi::allocate_cell(nibi::types::FUNCTION);
  case nibi::cell_type_e::ENVIRONMENT:
    return nibi::allocate_cell(nibi::types::ENVIRONMENT);
  case nibi::cell_type_e::SYMBOL:
    return nibi::allocate_cell(nibi::types::SYMBOL);
  default:
    return nibi::allocate_cell("unknown");
  }
}

} // namespace

cell_ptr builtin_fn_reflect_type(interpreter_c &ci, cell_list_t &list,
                                 env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::TYPE, ==, 2)
  return type(ci, list, env);
}

} // namespace builtins

} // namespace nibi
