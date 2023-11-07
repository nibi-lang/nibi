#include "machine/engine.hpp"

#include <iostream> // TODO: remove

namespace machine {

void engine_c::reset_instruction_handling() {
  // Clear process frames and reset 

  std::cout << "Engine asked to reset instruction handling" << std::endl;
}

void engine_c::handle_instructions(
  const bytes_t& instructions,
  instruction_error_handler_if& error_handler) {

  std::cout << "Engine got "
            << instructions.size()
            << " bytes of instructions to execute\n";

  // Need to come up with memory storage and object types to encapsulate 
  // stored items/ lists/ functions

  // for now, assume no unloading of tracers/files

  
}

execution_error_s engine_c::generate_error(
  const std::string& msg) const {
  // TODO: Add other debug info`
  return {msg};
}

} // namespace
