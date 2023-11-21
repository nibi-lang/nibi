#pragma once

#include "types.hpp"
#include "atoms.hpp"

#include <set>
#include <stack>
#include <string>

class atomiser_c {
public:
  atomiser_c() = delete;
  atomiser_c(
    const std::string& origin,
    list_receiver_if &receiver);

  void submit(const char* data, size_t line=0);
  void submit(std::string &data, size_t line=0);
  bool finish();

private:
  std::string _origin;
  list_receiver_if &_receiver;

  struct {
    size_t line{0};
    size_t col{0};
  } _trace;

  void emit_error(
    const std::string& message,
    const file_position_s& pos) {
    _receiver.on_error(
      file_error_s{ _origin, message, pos});
  }

  void emit_error(
    const std::string& message) {
    emit_error(
      message, 
      file_position_s{_trace.line, _trace.col});
  }

  void parse(std::string &string_data);
  void reset();

  bool collect_comments(std::string& data, atom_list_t* list);
  bool collect_number(std::string& data, atom_list_t* list);
  bool collect_string(std::string& data, atom_list_t* list);
  bool collect_identifier(std::string& data, atom_list_t* list);
  bool collect_symbol(std::string& data, atom_list_t* list);

  std::stack<atom_list_t> _active_lists;
  std::set<uint8_t> _termChars;
};


