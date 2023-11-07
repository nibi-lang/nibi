#include "intake.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "tracer.hpp"
#include "machine/engine.hpp"

#include <iostream>

#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>


namespace front {
namespace intake {

struct intake_group_s {
  bool in_repl{false};
  tracer_ptr tracer{nullptr};

  machine::engine_c engine;
  parser_c parser;
  lexer_c lexer;

  intake_group_s(tracer_ptr& tracer, bool in_repl=false)
    : in_repl{in_repl},
      tracer(tracer),
      parser(tracer, engine),
      lexer(parser) {}

};

// -------------------------------------------------

uint8_t repl(const settings_s& settings) {
  std::cerr << "Not yet complete\n";
  return 1;
}

// -------------------------------------------------

uint8_t dir(
    const settings_s& settings,
    const std::string& target) {
  std::cout << "front::intake::dir: " << target << std::endl;
  return 1;
}

// -------------------------------------------------

uint8_t file(
    const settings_s& settings,
    const std::string& target) {
  
  std::cout << "front::intake::file: " << target << std::endl;

  tracer_ptr tracer = front::allocate_tracer(target);
  intake_group_s ig(tracer);

  {
    std::filesystem::path path(target);
    if (!std::filesystem::is_regular_file(path)) {
      std::cout << "Not a regular file"
                << std::endl;
      return false;
    }
  }

  std::ifstream in(target);
  if (!in.is_open()) {
    std::cout << "Unable to open file"
              << std::endl;
    return false;
  }

  size_t line_no{1};
  std::string line;
  while(std::getline(in, line)) {
    ig.lexer.submit(line, line_no++);
  }

  in.close();

  if (!ig.lexer.finish()) {
    return -1;
  }

  return 0;
}

} // namespace
} // namespace
