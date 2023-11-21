#pragma once

#include <fmt/format.h>
#include <string>
#include <vector>
#include <memory>

class atom_c;
using atom_ptr = std::unique_ptr<atom_c>;
using atom_list_t = std::vector<atom_ptr>;

struct file_position_s {
  std::size_t line{0};
  std::size_t col{0};
  std::string to_string() {
    return fmt::format("({},{})", line, col);
  }
};

struct parse_group_s {
  std::string origin;
  std::vector<atom_list_t> lists;
};

struct file_error_s {
  std::string origin;
  std::string message;
  file_position_s pos;

  std::string to_string() {
    return fmt::format(
      "Error! origin({}):{} : {}",
      origin, pos.to_string(), message);
  }
};

