#pragma once

#include <fmt/format.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class atom_c;
using atom_ptr = std::unique_ptr<atom_c>;
using atom_list_t = std::vector<atom_ptr>;

struct file_position_s {
  std::size_t line{0};
  std::size_t col{0};
  std::string to_string() const {
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

class list_receiver_if {
public:
  virtual void on_list(atom_list_t list) = 0;
  virtual void on_error(file_error_s err) = 0;
};

enum class atom_type_e : uint8_t {
  SYMBOL,
  INTEGER,
  REAL,
  STRING,
  LIST
};

static const char* atom_type_to_string(const atom_type_e& type) {
  switch(type) {
    case atom_type_e::SYMBOL: return "symbol";
    case atom_type_e::INTEGER: return "integer";
    case atom_type_e::REAL: return "real";
    case atom_type_e::STRING: return "string";
    case atom_type_e::LIST: return "list";
  }
  return "<invalid>";
}

class atom_c {
public:
  virtual ~atom_c() = default;
  atom_c(const atom_type_e& type, const file_position_s& pos)
    : type(type), pos(pos) {}
  atom_type_e type;
  file_position_s pos{0,0};
  virtual std::string to_string() = 0;
};

class atom_symbol_c final : public atom_c {
public:
  atom_symbol_c(const std::string& data, const file_position_s& pos)
    : atom_c(atom_type_e::SYMBOL, pos),
      data(data){}
  std::string data;
  std::string to_string() override {
    return fmt::format(
      "type:{}, pos:{}, data:{}",
        atom_type_to_string(type),
        pos.to_string(),
        data);
  };
};

class atom_int_c final : public atom_c {
public:
  atom_int_c(const int64_t& data, const file_position_s& pos)
    : atom_c(atom_type_e::INTEGER, pos),
      data(data){}
  int64_t data{0};
  std::string to_string() override {
    return fmt::format(
      "type:{}, pos:{}, data:{}",
        atom_type_to_string(type),
        pos.to_string(),
        data);
  };
};

class atom_real_c final : public atom_c {
public:
  atom_real_c(const double& data, const file_position_s& pos)
    : atom_c(atom_type_e::REAL, pos),
      data(data){}
  double data{0};
  std::string to_string() override {
    return fmt::format(
      "type:{}, pos:{}, data:{}",
        atom_type_to_string(type),
        pos.to_string(),
        data);
  };
};

class atom_string_c final : public atom_c {
public:
  atom_string_c(const std::string& data, const file_position_s& pos)
    : atom_c(atom_type_e::STRING, pos),
      data(data){}
  std::string data;
  std::string to_string() override {
    return fmt::format(
      "type:{}, pos:{}, data:{}",
        atom_type_to_string(type),
        pos.to_string(),
        data);
  };
};

class atom_list_c final : public atom_c {
public:
  atom_list_c(atom_list_t data, const file_position_s& pos)
    : atom_c(atom_type_e::LIST, pos),
      data(std::move(data)){}
  atom_list_t data;
  std::string to_string() override {
    std::string data_str;
    for(const auto &atom : data) {
      data_str += fmt::format("{} ", atom->to_string());
    }
    return fmt::format(
      "type:{}, pos:{}, data:\n\t({})",
        atom_type_to_string(type),
        pos.to_string(),
        data_str);
  };
};

static void print_atom_list(atom_list_t &list) {
  for(auto& atom : list) {
    fmt::print("ATOM[{}] ", atom->to_string());
  }
}
