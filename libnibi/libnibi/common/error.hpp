#pragma once

#include "libnibi/source.hpp"

namespace nibi {

//! \brief An error that can be reported to the user.
class error_c {
public:
  //! \brief Create an error that doesn't contain a location.
  //! \param message The error message.
  error_c(const std::string message) : locator_(nullptr), message_(message) {}

  //! \brief Create an error.
  //! \param locator The locator interface.
  //! \param message The error message.
  error_c(locator_ptr locator, const std::string message)
      : locator_(locator), message_(message) {}

  //! \brief Check if the error has a locator interface.
  bool has_locator() const { return locator_ != nullptr; }

  //! \brief Get the locator interface.
  const locator_if *get_locator() const { return locator_.get(); }

  //! \brief Get the error message.
  const std::string get_message() const { return message_; }

  void draw_error(bool markup = true) const;

private:
  locator_ptr locator_{nullptr};
  std::string message_{""};
};

} // namespace nibi