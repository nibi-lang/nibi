#pragma once

#include "atoms.hpp"
#include <string>
#include <vector>
#include <memory>


struct variable_s {
  std::string name;
  file_position_s pos;
};

enum class parse_tree_node_e {
  FN,
  ASSIGNMENT,
  REASSIGNMENT,
};

class parse_node_c {
public:
  parse_node_c() = delete;
  parse_node_c(
    const parse_tree_node_e& ptne,
    const file_position_s& pos)
    : type(ptne),
      pos(pos){}

  parse_tree_node_e type;
  file_position_s pos;
};

using parse_node_ptr = std::unique_ptr<parse_node_c>;
using parse_list_t = std::vector<parse_node_c>; 

class fn_node_c : public parse_node_c {
public:
  fn_node_c() = delete;
  fn_node_c(
    const std::string& name,
    std::vector<variable_s> params,
    const file_position_s& pos)
    : parse_node_c(
        parse_tree_node_e::FN,
        pos),
      name(name),
      params(params),
      return_type(return_type) {}

    std::string name;
    std::vector<variable_s> params;
    datum_info_s return_type;
 
    parse_list_t body;
};
