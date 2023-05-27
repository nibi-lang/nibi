#include "lib.hpp"

#include <iostream>
#include <memory>
#include <string>

#include <filesystem>
#include <libnibi/macros.hpp>
#include <libnibi/platform.hpp>

nibi::cell_ptr get_argv(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                        nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{sys argv}", ==, 1)
  auto args = nibi::global_platform->get_program_args();
  auto argv_cell = nibi::allocate_cell(nibi::cell_type_e::LIST);
  auto &al = argv_cell->as_list();
  for (auto &arg : args) {
    al.push_back(nibi::allocate_cell(arg));
  }
  return argv_cell;
}

nibi::cell_ptr get_stdin(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                         nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{sys stdin}", ==, 1)
  auto args = nibi::global_platform->get_std_in();
  auto cell = nibi::allocate_cell(nibi::cell_type_e::LIST);
  auto &al = cell->as_list();
  for (auto &arg : args) {
    al.push_back(nibi::allocate_cell(arg));
  }
  return cell;
}

nibi::cell_ptr get_platform(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                            nibi::env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("{sys platform}", ==, 1)
  std::string platform_string = nibi::global_platform->get_platform_string();
  return nibi::allocate_cell(platform_string);
}
