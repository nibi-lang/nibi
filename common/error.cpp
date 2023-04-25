#include "error.hpp"
#include <iostream>

// TODO: Add markup support.

void error_c::draw_error(bool markup) const {

  if (!locator_) {
    std::cout << "error: " << message_ << std::endl;
    return;
  }

  auto [line, column] = locator_->get_line_column();
  auto source_name = locator_->get_source_name();
  std::cout << source_name << ":" << line << ":" << column
            << ": error: " << message_ << std::endl;
}