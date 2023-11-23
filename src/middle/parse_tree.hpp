#pragma once

enum class data_type_e {
  U8, U16, U32, U64,
  I8, I16, I32, I64,
  F32, F64,
  USER_DEFINED,
};

enum class data_classification_e {
  SINGLE, VEC
};

struct datum_info_s {
  data_type_e type{data_type_e::U8};
  data_classification_e classification
    {data_classification_e::SINGLE};
  bool is_ref{false};
};

struct variable_s {
  std::string name;
  datum_info_s info;
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
    const datum_info_s& return_type,
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
    uint32_t stack_requirements_bytes{0};
};
