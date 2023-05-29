#include "lib.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <filesystem>
#include <libnibi/macros.hpp>

namespace {
static bool loaded{false};
static std::vector<std::string> std_in;
void populate_std_in() {
  if (loaded) {
    return;
  }
  if (!isatty(STDIN_FILENO)) {
    std_in =
        std::vector<std::string>(std::istream_iterator<std::string>(std::cin),
                                 std::istream_iterator<std::string>());
  }
  loaded = true;
}
} // namespace

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
  populate_std_in();
  NIBI_LIST_ENFORCE_SIZE("{sys stdin}", ==, 1)
  auto args = std_in;
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
