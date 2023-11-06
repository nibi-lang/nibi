#include "machine/engine.hpp"

#include <iostream> // TODO: remove

namespace machine {

void engine_c::handle_instructions(
  const bytes_t& instructions,
  error_handler_ptr error_handler) {

  std::cout << "Engine got "
            << instructions.size()
            << " bytes of instructions to execute\n";
}

execution_error_s engine_c::generate_error(
  const std::string& msg) const {
  // TODO: Add other debug info`
  return {msg};
}

} // namespace
