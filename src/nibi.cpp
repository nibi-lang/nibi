#include "nibi.hpp"

#include "macros.hpp"
#include "object.hpp"
#include "keywords.hpp"
#include "bytecode/bytecode.hpp"
#include "rang.hpp"

#include "intake/input_buffer.hpp"
#include "intake/parser.hpp"


#include <iostream>
#include <string>

#ifndef NIBI_BUILD_HASH
#define NIBI_BUILD_HASH "unknown"
#endif

namespace nibi {
  const char* error_origin_to_string(const error_origin_e& e) {
    switch(e) {
      case error_origin_e::LEXER: return "LEXER";
      case error_origin_e::PARSER: return "PARSER";
      case error_origin_e::VM: return "VM";
    }
    return "UNKNOWN";
  }

  void fatal_error(const error_origin_e eo, locator_ptr loc, const std::string& msg) {
    global_runtime_flags.operational.store(false);
    std::cout << rang::fg::red << "\n[ RUNTIME HALT ]\n"
              << rang::fg::reset << std::endl;
    std::cerr << "Fatal error arose from: " << error_origin_to_string(eo) << std::endl;
    if (loc.get()) {
      draw_locator(*loc.get());
    }
    std::cerr << "\n" << msg << std::endl;

    shutdown_nibi();
  }

void shutdown_nibi() {
#ifdef DEBUG_BUILD
  std::cout << rang::fg::yellow << "Shutting down nibi" << rang::fg::reset << std::endl;
#endif

  if (global_runtime_flags.operational.load(std::memory_order_relaxed)) {
    global_runtime_flags.operational.store(false);
  }

  // TODO: The rest of shutdown
#ifdef DEBUG_BUILD
  std::cout << rang::fg::yellow << "Shutdown complete" << rang::fg::reset << std::endl;
#endif
}

} // namespace nibi

void show_version() {
  std::cout << "nibi version: " << NIBI_VERSION << std::endl;
  std::cout << "application build hash: " << NIBI_BUILD_HASH << std::endl;
}

void run_test_string() {

  nibi::source_manager_c sm;
  auto so = sm.get_source("test_string");

  nibi::parser_c parser;
  nibi::input_buffer_c buffer(parser);

  buffer.submit(so, "(+ 1 3.14159 (+ a b))", 1);

  buffer.end_ind();
}

int main(int argc, char** argv) {
#ifndef DEBUG_BUILD
  std::cout << rang::fg::cyan << "<DEBUG BUILD>" << rang::fg::reset << std::endl;
  show_version();
#endif

  run_test_string();

  return 0;
}
