#pragma once

#include "atoms.hpp"

namespace front { 

//! \brief Reads in a file and attempts to 
//!        parse it into a byte-encoded list of
//!        atoms
extern bool atomize_file(
  const std::string& file,
  std::vector<uint8_t>& dest);

}
