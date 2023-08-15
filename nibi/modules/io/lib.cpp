#include "lib.hpp"
#include <libnibi/macros.hpp>
#include <sstream>
#include <iostream>

nibi::cell_ptr nibi_flush(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env) {
  std::cout << std::flush;
 return nibi::allocate_cell(nibi::cell_type_e::NIL);
}

nibi::cell_ptr nibi_get_str(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{io get_str}", ==, 1)
  std::string line;
  std::getline(std::cin, line);
  return nibi::allocate_cell(line);
}

nibi::cell_ptr nibi_get_int(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{io get_int}", ==, 1)
  std::string line;
  std::getline(std::cin, line);
  int64_t val{0};
  try {
    val = std::stoll(line);
  } catch (std::out_of_range) {
    return nibi::allocate_cell(nibi::cell_type_e::NIL);
  } catch (std::invalid_argument) {
    return nibi::allocate_cell(nibi::cell_type_e::NIL);
  }
  auto r = nibi::allocate_cell((int64_t)val);
  r->locator = list[0]->locator;
  return r;
}

nibi::cell_ptr nibi_get_double(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{io get_doube}", ==, 1)
  std::string line;
  std::getline(std::cin, line);
  double val{0};
  try {
    val = std::stod(line);
  } catch (std::out_of_range) {
    return nibi::allocate_cell(nibi::cell_type_e::NIL);
  } catch (std::invalid_argument) {
    return nibi::allocate_cell(nibi::cell_type_e::NIL);
  }
  auto r = nibi::allocate_cell((double)val);
  r->locator = list[0]->locator;
  return r;
}

nibi::cell_ptr nibi_print(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{io print}", >=, 1)
 for(auto i = list.begin()+1; i != list.end(); i++) {
  std::cout << ci.process_cell((*i), env)->to_string();
 }
 return nibi::allocate_cell(nibi::cell_type_e::NIL);
}

nibi::cell_ptr nibi_println(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                               nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{io println}", >=, 1)
 for(auto i = list.begin()+1; i != list.end(); i++) {
  std::cout << ci.process_cell((*i), env)->to_string();
 }
 std::cout << "\n";
 return nibi::allocate_cell(nibi::cell_type_e::NIL);
}
