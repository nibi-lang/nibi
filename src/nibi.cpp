#include "nibi.hpp"

#include "macros.hpp"
#include "object.hpp"
#include "keywords.hpp"
#include "bytecode/bytecode.hpp"
#include "rang.hpp"
#include "vm/vm.hpp"

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

  // TODO: stop all proc_ifs

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

#ifdef DEBUG_BUILD
  std::cout << "Forcing debug to be true" << std::endl;
  bool debug = true;
#else
  bool debug = false;
#endif

  nibi::locator_table_c locator_table(debug);
  nibi::vm_c vm(locator_table);

  vm.start();

  nibi::parser_c parser(debug, locator_table, vm);
  nibi::input_buffer_c buffer(parser);

  buffer.submit(so, "(+ 1 3.14159 ", 1);
  buffer.submit(so, "(+ a b)) (+ 1 2", 2);
  buffer.submit(so, ")", 3);
  buffer.submit(so, "(- 10 2)", 4);

  buffer.end_ind();

  std::cout << "nibi.cpp >>> " << locator_table.size() << " locators created" << std::endl;

  vm.stop();
}

int main(int argc, char** argv) {
#ifdef DEBUG_BUILD
  std::cout << rang::fg::cyan << "<DEBUG BUILD>" << rang::fg::reset << std::endl;
  show_version();
#endif

  run_test_string();

  nibi::shutdown_nibi();

  return 0;
}
