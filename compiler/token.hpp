#pragma once

#include "source.hpp"
#include <memory>

enum class token_e {
  NIL,
  L_PAREN,
  R_PAREN,
  L_BRACE,
  R_BRACE,
  L_BRACKET,
  R_BRACKET,
  VARIADIC,
  FN,
  LOOP,
  IF,
  LT,
  GT,
  EQ,
  LTE,
  GTE,
  ASSIGNMENT,
  RETURN,
  LEN,
  AT,
  VIEW,
  QUOTE,
  EVAL,
  DROP,
  TYPE,
  QUIT,
  REV,
  SUB,
  AWAIT,
  USE,
  LOAD,
  IS_NIL,
  IS_PROMISE,
  EXISTS,
  ARGS_LIST,
  PARAM_BIND,
  MODULE,
  PUBLIC,
  SCOPE,
  ADD,
  SUBTRACT,
  DIVIDE,
  MULTIPLY,
  POWER,
  MODULO,
  LEFT_SHIFT,
  RIGHT_SHIFT,
  LOGICAL_AND,
  LOGICAL_OR,
  LOGICAL_NOT,
  BITWISE_AND,
  BITWISE_OR,
  BITWISE_XOR,
  BITWISE_NOT,
  RAW_INTEGER,
  RAW_FLOAT,
  RAW_STRING
};

//! \brief A token.
class token_c {
public:
  //! \brief Create a token.
  //! \param locator The locator interface.
  //! \param token The token value.
  token_c(std::shared_ptr<locator_if> locator, const token_e token)
    : locator_(locator), token_(token) {}
  //! \brief Get the token value.
  const token_e get_token() const { return token_; }
  //! \brief Get the locator interface.
  const std::shared_ptr<locator_if> get_locator() const { return locator_; }
private:
  std::shared_ptr<locator_if> locator_{nullptr};
  token_e token_{token_e::NIL};
};