#include "error.hpp"
#include <iostream>

#include "libnibi/rang.hpp"

namespace nibi {

void error_c::draw(bool markup) const {

  if (!locator_ || !markup) {
    std::cout << rang::fg::red << "ERROR: " << rang::fg::reset << message_
              << std::endl;
    return;
  }

  draw_locator(*locator_);

  std::cout << rang::fg::cyan << "\nMessage: " << rang::fg::reset << message_
            << "\n"
            << std::endl;
}
} // namespace nibi
