#pragma once

#include <memory>
#include <string>
#include <vector>

namespace parser {

enum class atom_type_e {
  UNDEFINED = 0,
  SYMBOL,       // Symbols are some chunk of text, use meta_e to classify 
  INTEGER,      // Note: We don't care about size of integers in the parser
  REAL,
  STRING,
  CHAR,
  LOAD_ARG,    // Pull result of previous computation as an argument
};

enum class meta_e {
  UNDEFINED,
  IDENTIFIER,
  TILDE, BACK_TICK,
  EXCLAMATION_POINT,
  AT, DOLLAR, MOD, HAT, AMPERSAND,
  ASTERISK, SUB, PLUS, EQUAL, NOT_EQUAL,
  LEFT_CURLY, RIGHT_CURLY, LEFT_BRACKET, RIGHT_BRACKET,
  PIPE, OR, COLON, COMMA, PERIOD,
  LT, GT, LTE, GTE, LSH, RSH, QUESTION_MARK, FORWARD_SLASH,
  SCOPE,
  EQUAL_EQUAL, SINGLE_QUOTE
};

class atom_c {
public:
  atom_c(const atom_type_e atom_type,
              const meta_e meta,
              std::string data,
              const size_t line,
              const size_t col)
    : line(line),
      col(col),
      atom_type(atom_type),
      meta(meta),
      data(data) {}
  size_t line{0};
  size_t col{0};
  atom_type_e atom_type{atom_type_e::UNDEFINED};
  meta_e meta{meta_e::UNDEFINED};
  std::string data;
};

struct error_s {
  std::string message;
  size_t line{0};
  size_t col{0};
};

using list_t = std::vector<std::unique_ptr<atom_c>>;

} // namespace
