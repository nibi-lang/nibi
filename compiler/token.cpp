#include "token.hpp"
#include <iostream>

const char* token_to_string(const token_c& token) {
  switch(token.get_token()) {
    case token_e::NIL: return "NIL";
    case token_e::L_PAREN: return "L_PAREN";
    case token_e::R_PAREN: return "R_PAREN";
    case token_e::L_BRACE: return "L_BRACE";
    case token_e::R_BRACE: return "R_BRACE";
    case token_e::L_BRACKET: return "L_BRACKET";
    case token_e::R_BRACKET: return "R_BRACKET";
    case token_e::VARIADIC: return "VARIADIC";
    case token_e::FN: return "FN";
    case token_e::LOOP: return "LOOP";
    case token_e::IF: return "IF";
    case token_e::LT: return "LT";
    case token_e::GT: return "GT";
    case token_e::EQ: return "EQ";
    case token_e::LTE: return "LTE";
    case token_e::GTE: return "GTE";
    case token_e::ASSIGNMENT: return "ASSIGNMENT";
    case token_e::RETURN: return "RETURN";
    case token_e::LEN: return "LEN";
    case token_e::AT: return "AT";
    case token_e::VIEW: return "VIEW";
    case token_e::QUOTE: return "QUOTE";
    case token_e::EVAL: return "EVAL";
    case token_e::DROP: return "DROP";
    case token_e::TYPE: return "TYPE";
    case token_e::QUIT: return "QUIT";
    case token_e::REV: return "REV";
    case token_e::SUB: return "SUB";
    case token_e::AWAIT: return "AWAIT";
    case token_e::USE: return "USE";
    case token_e::LOAD: return "LOAD";
    case token_e::IS_NIL: return "IS_NIL";
    case token_e::IS_PROMISE: return "IS_PROMISE";
    case token_e::EXISTS: return "EXISTS";
    case token_e::ARGS_LIST: return "ARGS_LIST";
    case token_e::PARAM_BIND: return "PARAM_BIND";
    case token_e::MODULE: return "MODULE";
    case token_e::PUBLIC: return "PUBLIC";
    case token_e::SCOPE: return "SCOPE";
    case token_e::ADD: return "ADD";
    case token_e::SUBTRACT: return "SUBTRACT";
    case token_e::DIVIDE: return "DIVIDE";
    case token_e::MULTIPLY: return "MULTIPLY";
    case token_e::POWER: return "POWER";
    case token_e::MODULO: return "MODULO";
    case token_e::L_SHIFT: return "L_SHIFT";
    case token_e::R_SHIFT: return "R_SHIFT";
    case token_e::LOGICAL_AND: return "LOGICAL_AND";
    case token_e::LOGICAL_OR: return "LOGICAL_OR";
    case token_e::LOGICAL_NOT: return "LOGICAL_NOT";
    case token_e::BITWISE_AND: return "BITWISE_AND";
    case token_e::BITWISE_OR: return "BITWISE_OR";
    case token_e::BITWISE_XOR: return "BITWISE_XOR";
    case token_e::BITWISE_NOT: return "BITWISE_NOT";
    case token_e::RAW_INTEGER: return "RAW_INTEGER";
    case token_e::RAW_FLOAT: return "RAW_FLOAT";
    case token_e::RAW_STRING: return "RAW_STRING";
    case token_e::IDENTIFIER: return "IDENTIFIER";
    case token_e::INSIST_NIL: return "INSIST_NIL";
    case token_e::INSIST_INT: return "INSIST_INT";
    case token_e::INSIST_FLOAT: return "INSIST_FLOAT";
    case token_e::INSIST_STRING: return "INSIST_STRING";
    case token_e::INSIST_PROMISE: return "INSIST_PROMISE";
    case token_e::INSIST_FN: return "INSIST_FN";
  }
  return "<<<UNKNOWN>>>";
}





