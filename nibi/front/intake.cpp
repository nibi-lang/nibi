#include "intake.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "tracer.hpp"
#include "machine/engine.hpp"

#include <iostream>

#include <fstream>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>


namespace front {
namespace intake {

namespace {

struct intake_group_s {
  traced_file_ptr traced_file{nullptr};
  tracer_ptr tracer{nullptr};

  machine::engine_c engine;
  parser_c parser;
  lexer_c lexer;

  intake_group_s(
      traced_file_ptr& traced_file, 
      runtime::context_c &ctx,
      bool in_repl=false)
    : traced_file(traced_file),
      tracer(traced_file->get_tracer()),
      engine(ctx.get_memory_core()),
      parser(tracer, engine),
      lexer(parser) {

      engine.set_print_results(in_repl);

      // Tracers may need to live beyond parsers, 
      // so we give them a ref to the trace stuff
      ctx.add_traced_file(traced_file);
      ctx.add_tracer(traced_file->get_name(), tracer);
    }
};

} // namespace


// -------------------------------------------------

uint8_t repl(settings_s& settings) {

  traced_file_ptr traced_file = front::allocate_traced_file("REPL");
  intake_group_s ig(traced_file, settings.ctx, true);

  std::string line;
  size_t line_no{1};
  while (std::getline(std::cin, line)) {
    ig.lexer.submit(line, line_no++);
  }
  return 0;
}

// -------------------------------------------------

uint8_t dir(
    settings_s& settings,
    const std::string& target) {
  std::cout << "NOT YET COMPLETE" << std::endl;
  std::cout << "front::intake::dir: " << target << std::endl;
  return 1;
}

// -------------------------------------------------

uint8_t file(
    settings_s& settings,
    const std::string& target) {
  
  std::cout << "front::intake::file: " << target << std::endl;

  traced_file_ptr traced_file = front::allocate_traced_file(target);
  intake_group_s ig(traced_file, settings.ctx);

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
