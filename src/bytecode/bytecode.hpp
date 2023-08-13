#pragma once

#include <cstring>
/*
    Bytecode Objects
    The objects here closely resemble the Nibi data structures as the
    data and instruction set of a LISP are very similar. However,
    the bytecode described here is a near 1-1 translation from the
    user provided code meaning there isn't complex structures
    that need to be executed to be generated.

*/

#include <string>

namespace bytecode {

enum class op_e {
  NOP,
  BUILTIN_SYMBOL,   // integer encoded builtin command id
  USER_SYMBOL, 
  TAG,              // : prefixed tag
  INTEGER,
  REAL,
  CHAR,
  LIST_INS_IND,     // (  - int length
  LIST_DATA_IND,    // [  - int length
  LIST_ACCESS_IND,  // {  - int length
  LOCATOR           // Source locator
};

union data_u {
  char* s;
  int64_t i;
  double d;
  char c;
};

struct instruction_s {

  op_e op{op_e::NOP};
  data_u data;

  instruction_s(op_e op) : op(op) { this->data.i = 0; }
  instruction_s() : op(op_e::NOP) { this->data.i = 0; }
  instruction_s(int64_t i) : op(op_e::INTEGER) { this->data.i = i; }
  instruction_s(double d) : op(op_e::REAL) { this->data.d = d; }
  instruction_s(char c) : op(op_e::CHAR) { this->data.c = c; }
  instruction_s(op_e, const std::string& s) : op(op) {
    this->data.s = new char[s.size()+1];
    strncpy(this->data.s, s.data(), s.size());
    this->data.s[s.size()] = '\0';
  }

  bool contains_string() const {
    return ((this->op == op_e::BUILTIN_SYMBOL ||
            this->op == op_e::USER_SYMBOL ||
            this->op == op_e::TAG) && this->data.c);
  }

  ~instruction_s() {
    if (contains_string()) {
      delete [] this->data.s;
    }
  }
};

} // namespace
