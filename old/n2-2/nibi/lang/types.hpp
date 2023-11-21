#pragma once

#include <fmt/format.h>
#include <string>
#include <vector>
#include <memory>

struct file_position_s {
  std::size_t line{0};
  std::size_t col{0};
  std::string to_string() {
    return fmt::format("({},{})", line, col);
  }
};

class atom_c;
using atom_ptr = std::unique_ptr<atom_c>;
using atom_list_t = std::vector<atom_ptr>;

struct file_s {
  std::string name;
  std::vector<atom_list_t> lists;
};

struct error_s {
  std::string file;
  std::string message;
  file_position_s pos;
};
