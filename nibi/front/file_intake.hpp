#pragma once

#include "atoms.hpp"

// TODO:
//
//    1) Make an instruction_receiver_if
//    2) Change this atom_receiver_if to an instruction_receiver_if 
//    3) Create a parser in the cpp, and pass the instruction_receiver_if to it
//    4) Update the main bit to receive instructions
//


namespace front {

//! \brief Attempt to parse a file
//! \returns true iff the parser was able to consume
//!          the file entirely
//! \note Data and errors reported to receiver
extern bool from_file(
  front::atom_receiver_if &receiver,
  const std::string &file_name);

} // namespace
