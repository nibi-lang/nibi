#include "lib.hpp"

#include <iostream>
#include <memory>
#include <string>

nibi::cell_ptr get_str(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                       nibi::env_c &env) {
  std::string line;
  std::getline(std::cin, line);
  return nibi::allocate_cell(line);
}

nibi::cell_ptr get_int(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                       nibi::env_c &env) {
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
  return nibi::allocate_cell((int64_t)val);
}

nibi::cell_ptr get_double(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                       nibi::env_c &env) {
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
  return nibi::allocate_cell((double)val);
}