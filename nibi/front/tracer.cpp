#include "tracer.hpp"

#include <iostream>

namespace front {

void traced_file_c::trace_error(
  const pos_s& pos,
  machine::execution_error_s error) {

  std::cout << "NOT YET IMPLEMENTED" << std::endl;

  std::cout << "Tracer asked to trace file: "
            << _file_name 
            << "\n"
            << "Line: "
            << pos.line
            << ", Col: "
            << pos.col
            << "\nMessage: "
            << error.message
            << std::endl;
}

} // namespace
