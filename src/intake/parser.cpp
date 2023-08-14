#include "parser.hpp"
#include "nibi.hpp"
#include <iostream>
#include <cassert>

namespace nibi {

#define INSERT_LOCATOR_INS \
  instructions_.emplace_back(\
      bytecode::instruction_s(\
        new bytecode::locator_wrapper_s{\
          current_location()}));

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

parser_c::parser_c() {
  // Copy locally so we don't jump far when we
  // check for debug mode
  debug_ = global_runtime_settings.debug_enabled;
}

void parser_c::tokens_ind(std::vector<token_c> &tokens) {
  tokens_ = &tokens;

  std::cout << "Got " << tokens.size() << " tokens\n";

  for(auto t : tokens) {

    std::cout << "\t" << token_to_string(t) << std::endl;
  }

  index_ = 0;
  instructions_.clear();

  if (!instruction_list()) {
    fatal_error(error_origin_e::PARSER, tokens[0].get_locator(), "Invalid instruction list - expected '('");
    return;
  }

  if (instructions_.empty()) {
    return;
  }

  // TODO: SEND THIS >>>>>>>>>>>>>>>>  vm.instructions_ind(list);

  return;
}

bool parser_c::instruction_list(){
  if (tokens_->empty()) { return false; }
  
  if (current_token() != token_e::L_PAREN) {
    return false;
  }

  INSERT_LOCATOR_INS;

  next();

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

  next();

  return true;
}

bool parser_c::access_list(){
  if (current_token() != token_e::L_BRACE) {
    return false;
  }

  CONDITIONAL_DEBUG_SYMBOL_INSERTION;

  next();

  PARSER_SCAN_LIST(token_e::R_BRACE, symbol);

  next();

  return true;
}
bool parser_c::data_list(){

  if (current_token() != token_e::L_BRACKET) {
    return false;
  }

  CONDITIONAL_DEBUG_SYMBOL_INSERTION;

  next();

  PARSER_SCAN_LIST(token_e::R_BRACKET, element);

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

bool parser_c::data(){ return false; }
bool parser_c::element(){ return false; }
bool parser_c::symbol(){ return false; }
bool parser_c::number(){ return false; }
bool parser_c::integer(){ return false; }
bool parser_c::boolean(){ return false; }
bool parser_c::real(){ return false; }
bool parser_c::string(){ return false; }
bool parser_c::cchar(){ return false; }

} // namespace nibi
