#pragma once

#include "interfaces.hpp"

namespace parser {

//! \brief Attempt to parse a file
//! \returns true iff the parser was able to consume
//!          the file entirely
//! \note Data and errors reported to receiver
extern bool from_file(
  parser::receiver_if &receiver,
  const std::string &file_name);

} // namespace
