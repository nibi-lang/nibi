#include "token.hpp"
#include <iostream>

const char *token_to_string(const token_c &token) {
  switch (token.get_token()) {
  case token_e::L_PAREN:
    return "L_PAREN";
  case token_e::R_PAREN:
    return "R_PAREN";
  case token_e::L_BRACKET:
    return "L_BRACKET";
  case token_e::R_BRACKET:
    return "R_BRACKET";
  case token_e::L_BRACE:
    return "L_BRACE";
  case token_e::R_BRACE:
    return "R_BRACE";
  case token_e::RAW_INTEGER:
    return "RAW_INTEGER";
  case token_e::RAW_FLOAT:
    return "RAW_FLOAT";
  case token_e::RAW_STRING:
    return "RAW_STRING";
  case token_e::SYMBOL:
    return "SYMBOL";
  case token_e::NIL:
    return "NIL";
  }
  return "<<<UNKNOWN>>>";
}
