#include "lib.hpp"

#include <iostream>
#include <libnibi/common/error.hpp>
#include <libnibi/interpreter/interpreter.hpp>
#include <memory>
#include <string>

#define CHECK_SIZE(op, n)                                                      \
  if (list.size() op n) {                                                      \
    nibi::global_interpreter->halt_with_error(nibi::error_c(                   \
        list.front()->locator,                                                 \
        "instruction expects " + std::to_string(n - 1) + " parameters, got " + \
            std::to_string(list.size() - 1) + "."));                           \
  }

nibi::cell_ptr get_str(nibi::cell_list_t &list, nibi::env_c &env) {
  std::string line;
  std::getline(std::cin, line);
  return nibi::allocate_cell(line);
}

nibi::cell_ptr type(nibi::cell_list_t &list, nibi::env_c &env) {
  CHECK_SIZE(!=, 2)

  auto resolved = nibi::global_interpreter->execute_cell(list[1], env);
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

nibi::cell_ptr is_numeric(nibi::cell_list_t &list, nibi::env_c &env) {
  CHECK_SIZE(!=, 2)
  return nibi::allocate_cell(
      (int64_t)nibi::global_interpreter->execute_cell(list[1], env)
          ->is_numeric());
}

nibi::cell_ptr is_int(nibi::cell_list_t &list, nibi::env_c &env) {
  return nibi::allocate_cell(
      (int64_t)(nibi::global_interpreter->execute_cell(list[1], env)->type ==
                nibi::cell_type_e::INTEGER));
}

nibi::cell_ptr is_double(nibi::cell_list_t &list, nibi::env_c &env) {
  return nibi::allocate_cell(
      (int64_t)(nibi::global_interpreter->execute_cell(list[1], env)->type ==
                nibi::cell_type_e::DOUBLE));
}

nibi::cell_ptr is_string(nibi::cell_list_t &list, nibi::env_c &env) {
  return nibi::allocate_cell(
      (int64_t)(nibi::global_interpreter->execute_cell(list[1], env)->type ==
                nibi::cell_type_e::STRING));
}

nibi::cell_ptr is_list(nibi::cell_list_t &list, nibi::env_c &env) {
  return nibi::allocate_cell(
      (int64_t)(nibi::global_interpreter->execute_cell(list[1], env)->type ==
                nibi::cell_type_e::LIST));
}

nibi::cell_ptr is_nil(nibi::cell_list_t &list, nibi::env_c &env) {
  return nibi::allocate_cell(
      (int64_t)(nibi::global_interpreter->execute_cell(list[1], env)->type ==
                nibi::cell_type_e::NIL));
}
