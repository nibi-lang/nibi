#include "tracer.hpp"

#include <iostream>

namespace front {

void traced_file_c::trace_error(
  const pos_s& pos,
  const std::string& message,
  bool is_fatal) {

  std::cout << "NOT YET IMPLEMENTED" << std::endl;

  std::cout << "Tracer asked to trace file: "
            << _file_name 
            << "\n"
            << "Line: "
            << pos.line
            << ", Col: "
            << pos.col
            << "\nMessage: "
            << message
            << std::endl;

  if (is_fatal) {
    std::exit(1);
  }
}

} // namespace
