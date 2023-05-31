#pragma once

#include "libnibi/source.hpp"
#include <memory>

namespace nibi {

enum class token_e {
  NIL,
  TRUE,
  FALSE,
  NOT_A_NUMBER,
  L_PAREN,
  R_PAREN,
  L_BRACKET,
  R_BRACKET,
  L_BRACE,
  R_BRACE,
  RAW_INTEGER,
  RAW_FLOAT,
  RAW_STRING,
  SYMBOL,
};

//! \brief A token.
class token_c {
public:
  //! \brief Create a token.
  //! \param locator The locator interface.
  //! \param token The token value.
  token_c(locator_ptr locator, const token_e token)
      : locator_(locator), token_(token) {}
  //! \brief Create a token.
  //! \param locator The locator interface.
  //! \param token The token value.
  //! \param data The data associated with the token.
  token_c(locator_ptr locator, const token_e token, const std::string &data)
      : locator_(locator), token_(token), data_(data) {}
  //! \brief Get the token value.
  const token_e get_token() const { return token_; }
  //! \brief Get the locator interface.
  const locator_ptr get_locator() const { return locator_; }
  //! \brief Get the data associated with the token.
  const std::string get_data() const { return data_; }

private:
  locator_ptr locator_{nullptr};
  token_e token_{token_e::NIL};
  std::string data_{""};
};

extern const char *token_to_string(const token_c &token);

} // namespace nibi
