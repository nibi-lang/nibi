#include "parser.hpp"
#include "nibi.hpp"
#include "maps.hpp"
#include <iostream>
#include <cassert>

namespace nibi {

#define INSERT_LOCATOR_INS \
  if (current_location()) { \
    auto v = locator_table_.add_locator(current_location()); \
  instructions_.emplace_back(\
      bytecode::instruction_s(\
        bytecode::locator_table_wrapper_s{v}));\
  }

#define CONDITIONAL_DEBUG_SYMBOL_INSERTION \
  if (debug_) { \
    INSERT_LOCATOR_INS;\
  }

#define PARSER_SCAN_LIST(sym_close, fn) \
  auto loc = current_location();\
  while(current_token() != sym_close) {\
    if (!fn()) {\
      std::string msg = "Invalid list - Expected value (symbol, number, "\
                        "string) or list (instruction, access, data)";\
      msg += "\n Got: " + std::string(token_to_string((*tokens_)[index_]));\
      msg += "\n It is possible that this is a result of a missing closing "\
             "symbol in a previous instruction list";\
      fatal_error(error_origin_e::PARSER, loc, msg);\
      return false;\
    }\
    if (!has_next()){\
      fatal_error(error_origin_e::PARSER, current_location(), "Invalid instruction - Unexpected end of input");\
      return false;\
    }\
    loc = current_location();\
  }\
  if (current_token() != sym_close) {\
    std::string msg = "Expected closing '" +\
                      closing_sym_from_token(sym_close) + "' symbol";\
    fatal_error(error_origin_e::PARSER, current_location(), msg);\
    return false;\
  }

namespace {
inline std::string closing_sym_from_token(const token_e token) {
  assert((token == token_e::R_PAREN || token == token_e::R_BRACKET ||
          token == token_e::R_BRACE));
  switch (token) {
  case token_e::L_PAREN:
    return ")";
  case token_e::L_BRACKET:
    return "]";
  case token_e::L_BRACE:
    return "}";
  default:
    return "";
  }
}
} // namespace

parser_c::parser_c(const bool debug, locator_table_c &table, vm_c &vm)
  : debug_(debug), locator_table_(table), vm_(vm) {
}

void parser_c::tokens_ind(std::vector<token_c> &tokens) {
  tokens_ = &tokens;

  index_ = 0;
  instructions_.clear();

  INSERT_LOCATOR_INS;

  while (instruction_list()) {}

  if (instructions_.empty()) {
    fatal_error(error_origin_e::PARSER, nullptr, "Expected instruction list");
    return;
  }

  vm_.bytecode_ind(instructions_);
  return;
}

bool parser_c::instruction_list(){
  if (tokens_->empty()) { return false; }
  
  if (current_token() != token_e::L_PAREN) {
    return false;
  }

  next();

  instructions_.emplace_back(
      bytecode::op_e::LIST_INS_IND);

  bool first_ins {false};
  switch(current_token()) {
    case token_e::SYMBOL: first_ins = symbol(); break;
    case token_e::L_BRACE: first_ins = access_list(); break;
    case token_e::L_PAREN: first_ins = instruction_list(); break;
    default:
      fatal_error(error_origin_e::PARSER, current_location(),
                      "Invalid instruction list - Expected symbol, access "
                      "list, or instruction list. Got: " +
                          std::string(token_to_string((*tokens_)[index_])));
      return false;
  }

  PARSER_SCAN_LIST(token_e::R_PAREN, element);

  instructions_.emplace_back(
      bytecode::op_e::LIST_END);

  next();

  return true;
}

bool parser_c::access_list(){
  if (current_token() != token_e::L_BRACE) {
    return false;
  }

  CONDITIONAL_DEBUG_SYMBOL_INSERTION;

  next();

  instructions_.emplace_back(
      bytecode::op_e::LIST_DATA_IND);

  PARSER_SCAN_LIST(token_e::R_BRACE, symbol);

  instructions_.emplace_back(
      bytecode::op_e::LIST_END);

  next();

  return true;
}
bool parser_c::data_list(){

  if (current_token() != token_e::L_BRACKET) {
    return false;
  }

  CONDITIONAL_DEBUG_SYMBOL_INSERTION;

  next();

  instructions_.emplace_back(
      bytecode::op_e::LIST_ACCESS_IND);

  PARSER_SCAN_LIST(token_e::R_BRACKET, element);

  instructions_.emplace_back(
      bytecode::op_e::LIST_END);

  next();

  return true;
}

bool parser_c::list(){

  bool acquired = instruction_list();
  if (acquired) {
    return true;
  }

  acquired = data_list();
  if (acquired) {
    return true;
  }

  return access_list();
}

bool parser_c::data(){

  auto data = symbol();
  if (data) {
    return true;
  }

  data = number();
  if (data) {
    return true;
  }

  data = string();
  if (data) {
    return true;
  }

  data = cchar();
  if (data) {
    return true;
  }

  return nil();
}

bool parser_c::element(){
  auto element = data();
  if (element) {
    return true;
  }

  return list();
}

bool parser_c::symbol(){
  if (current_token() != token_e::SYMBOL) {
    return false;
  }

  CONDITIONAL_DEBUG_SYMBOL_INSERTION;

  auto symbol_raw = current_data();

  // Check if builtin
  {
    auto fbi = global_symbol_id_map.find(symbol_raw);
    if (fbi != global_symbol_id_map.end()) {
      instructions_.emplace_back(
          bytecode::op_e::BUILTIN_SYMBOL,
          fbi->second);
      next();
      return true;
    }
  }

  // check if tag
  if (symbol_raw[0] == ':') {
    auto fti = global_trivial_tag_map.find(symbol_raw);
    if (fti != global_trivial_tag_map.end()) {
      instructions_.emplace_back(
          bytecode::op_e::TAG,
          (int64_t)fti->second);
      next();
      return true;
    }
  }

  instructions_.emplace_back(
      bytecode::op_e::USER_SYMBOL,
      symbol_raw);
  next();
  return true;
}

bool parser_c::number(){
  auto num = integer();
  if (num) {
    return true;
  }
  num = real();
  if (num) {
    return true;
  }
  return boolean();
}

bool parser_c::integer(){
  if (current_token() != token_e::RAW_INTEGER) {
    return false;
  }

  auto stringed_value = current_data();
  
  int64_t value_actual{0};
  try {
    value_actual = std::stoll(stringed_value);
  } catch (std::exception &e) {
    fatal_error(error_origin_e::PARSER, current_location(), {"Invalid integer value: " + stringed_value});
    return false;
  }

  CONDITIONAL_DEBUG_SYMBOL_INSERTION;

  instructions_.emplace_back(value_actual);

  next();

  return true;
}

bool parser_c::boolean(){
  if (current_token() != token_e::TRUE && current_token() != token_e::FALSE) {
    return false;
  }
  
  CONDITIONAL_DEBUG_SYMBOL_INSERTION;

  instructions_.emplace_back((int64_t)(current_token() == token_e::TRUE));

  next();

  return true;
}

bool parser_c::real(){
  if (current_token() != token_e::RAW_FLOAT &&
      current_token() != token_e::NOT_A_NUMBER &&
      current_token() != token_e::INF) {
    return false;
  }

  auto stringed_value = current_data();

  double value_actual{0.00};
  try {
    value_actual = std::stod(stringed_value);
  } catch (std::exception &e) {
    fatal_error(error_origin_e::PARSER, current_location(), {"Invalid double value: " + stringed_value});
    return false;
  }

  CONDITIONAL_DEBUG_SYMBOL_INSERTION;

  instructions_.emplace_back(value_actual);

  next();

  return true;
}

bool parser_c::string(){
  if (current_token() != token_e::RAW_STRING) {
    return false;
  }

  CONDITIONAL_DEBUG_SYMBOL_INSERTION;

  instructions_.emplace_back(
      bytecode::op_e::STRING,
      current_data());
 
  next();
  
  return true;
}

bool parser_c::cchar(){
  if (current_token() != token_e::RAW_CHAR) {
    return false;
  }

  auto data = current_data();
  char data_as_char = '\0';

  if (data.size()) {
    data_as_char = data[0];
  }

  if (data.size() != 1) {
    fatal_error(error_origin_e::PARSER, current_location(), {"Invalid char value: " + data});
    return false;
  }

  CONDITIONAL_DEBUG_SYMBOL_INSERTION;

  instructions_.emplace_back(
      data_as_char);

  next();

  return true;
}

bool parser_c::nil(){
  if (current_token() != token_e::NIL) {
    return false;
  }

  instructions_.emplace_back(
    bytecode::op_e::NIL);

  next();

  return true;
}

} // namespace nibi
