#pragma once

#include "types.hpp"
#include <memory>
#include <string>
#include <vector>

namespace front {

enum class atom_type_e {
  UNDEFINED = 0,
  SYMBOL,       // Symbols are some chunk of text, use meta_e to classify 
  INTEGER,      // Note: We don't care about size of integers in the parser
  REAL,
  STRING,
  LOAD_ARG,    // Pull result of previous computation as an argument
};

enum class meta_e {
  UNDEFINED = 0,
  IDENTIFIER,
  TILDE, BACK_TICK,
  EXCLAMATION_POINT,
  AT, DOLLAR, MOD, HAT, AMPERSAND,
  ASTERISK, SUB, PLUS, EQUAL, NOT_EQUAL,
  LEFT_CURLY, RIGHT_CURLY, LEFT_BRACKET, RIGHT_BRACKET,
  PIPE, OR,
  LT, GT, LSH, RSH, QUESTION_MARK, FORWARD_SLASH,
  EQUAL_EQUAL, SINGLE_QUOTE,
  LEFT_ARROW,
  TAG
};

class atom_c {
public:
  atom_c(const atom_type_e atom_type,
              const meta_e meta,
              std::string data,
              const size_t line,
              const size_t col)
    : pos(line, col),
      atom_type(atom_type),
      meta(meta),
      data(data) {}
  pos_s pos;
  atom_type_e atom_type{atom_type_e::UNDEFINED};
  meta_e meta{meta_e::UNDEFINED};
  std::string data;
};

using atom_ptr = std::unique_ptr<atom_c>;
using atom_list_t = std::vector<atom_ptr>;

extern void print_list(atom_list_t& list);

class atom_receiver_if {
public:
  //! \brief Called in the event of a parsing error
  virtual void on_error(error_s) = 0;
  //! \brief Called when a full list has been parsed
  virtual void on_list(atom_list_t) = 0;
  //! \brief Called when a top-most list has been completly parsed
  //!        after all potential sub-lists
  virtual void on_top_list_complete() = 0;
};

} // namespace
