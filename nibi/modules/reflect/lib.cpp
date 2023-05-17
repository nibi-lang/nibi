#include "lib.hpp"

#include <iostream>
#include <libnibi/common/error.hpp>
#include <libnibi/interpreter/interpreter.hpp>
#include <libnibi/list_helpers.hpp>
#include <memory>
#include <string>

nibi::cell_ptr type(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                    nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{reflect type}", ==, 2)
  auto resolved = ci.execute_cell(list[1], env);
  switch (resolved->type) {
  case nibi::cell_type_e::ABERRANT:
    return nibi::allocate_cell("aberrant");
  case nibi::cell_type_e::NIL:
    return nibi::allocate_cell("nil");
  case nibi::cell_type_e::INTEGER:
    return nibi::allocate_cell("int");
  case nibi::cell_type_e::DOUBLE:
    return nibi::allocate_cell("double");
  case nibi::cell_type_e::STRING:
    return nibi::allocate_cell("string");
  case nibi::cell_type_e::LIST:
    return nibi::allocate_cell("list");
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

nibi::cell_ptr is_numeric(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                          nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{reflect is_numeric}", ==, 2)
  return nibi::allocate_cell(
      (int64_t)ci.execute_cell(list[1], env)->is_numeric());
}

nibi::cell_ptr is_int(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                      nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{reflect is_int}", ==, 2)
  return nibi::allocate_cell((int64_t)(ci.execute_cell(list[1], env)->type ==
                                       nibi::cell_type_e::INTEGER));
}

nibi::cell_ptr is_double(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                         nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{reflect is_double}", ==, 2)
  return nibi::allocate_cell((int64_t)(ci.execute_cell(list[1], env)->type ==
                                       nibi::cell_type_e::DOUBLE));
}

nibi::cell_ptr is_string(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                         nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{reflect is_string}", ==, 2)
  return nibi::allocate_cell((int64_t)(ci.execute_cell(list[1], env)->type ==
                                       nibi::cell_type_e::STRING));
}

nibi::cell_ptr is_list(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                       nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{reflect is_list}", ==, 2)
  return nibi::allocate_cell((int64_t)(ci.execute_cell(list[1], env)->type ==
                                       nibi::cell_type_e::LIST));
}

nibi::cell_ptr is_nil(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                      nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{reflect is_nil}", ==, 2)
  return nibi::allocate_cell(
      (int64_t)(ci.execute_cell(list[1], env)->type == nibi::cell_type_e::NIL));
}
