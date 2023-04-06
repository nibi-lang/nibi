#pragma once

#include "source.hpp"

//! \brief An error that can be reported to the user.
class error_c {
public:
  //! \brief Create an error.
  //! \param locator The locator interface.
  //! \param message The error message.
  error_c(std::unique_ptr<locator_if> locator, const std::string message)
    : locator_(std::move(locator)), message_(message) {}

  //! \brief Get the locator interface.
  const locator_if* get_locator() const {
    return locator_.get();
  }

  //! \brief Get the error message.
  const std::string get_message() const {
    return message_;
  }

  void draw_error(bool markup = true) const;
private:
  std::unique_ptr<locator_if> locator_{nullptr};
  std::string message_{""};
};