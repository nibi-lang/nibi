#include "libnibi/source.hpp"
#include "libnibi/rang.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

namespace nibi {
void draw_locator(locator_if &location) {

  std::cout << rang::fg::magenta << location.get_source_name()
            << rang::fg::reset << " : (" << rang::fg::blue
            << location.get_line() << rang::fg::reset << "," << rang::fg::blue
            << location.get_column() << rang::fg::reset << ")\n";

  struct line_data_pair_s {
    uint64_t number;
    std::string data;
  };

  std::ifstream fs;
  fs.open(location.get_source_name());

  if (!fs.is_open()) {
    return;
  }

  // A window of source
  std::vector<line_data_pair_s> window;

  // Get to the line
  std::string line_data;
  uint64_t line_number{0};

  // Determine the upper and lower bound for a source code window
  int64_t upper_bound = location.get_line() + 4;
  int64_t lower_bound = (int64_t)location.get_line() - 5;
  if (lower_bound < 0) {
    lower_bound = 0;
  }

  // Build a window of source code to display
  while (std::getline(fs, line_data)) {
    line_number++;
    if ((line_number >= lower_bound && lower_bound < location.get_line()) ||
        location.get_line() == line_number ||
        line_number > location.get_line() && line_number < upper_bound) {
      window.push_back({.number = line_number, .data = line_data});
    }

    if (line_number >= upper_bound) {
      break;
    }
  }

  // Determine the alignment
  size_t width = 2;
  {
    auto s = std::to_string(upper_bound);
    if (s.length() + 1 > width) {
      width = s.length() + 1;
    }
  }

  // Make an arrow to show where the error is
  std::string pointer;
  for (size_t i = 0; i < location.get_column(); i++) {
    pointer += "~";
  }
  pointer += "^";

  // Draw the window
  for (auto line_data : window) {
    if (line_data.number == location.get_line()) {
      std::cout << rang::fg::yellow << std::right << std::setw(width)
                << line_data.number << rang::fg::reset << " | "
                << line_data.data << std::endl;
      std::cout << rang::fg::cyan << std::right << std::setw(width) << ">>"
                << rang::fg::reset << " | " << rang::fg::red << pointer
                << rang::fg::reset << std::endl;
    } else {
      std::cout << rang::fg::green << std::right << std::setw(width)
                << line_data.number << rang::fg::reset << " | "
                << line_data.data << std::endl;
    }
  }
  fs.close();
}
}