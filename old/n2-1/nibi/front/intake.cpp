#include "intake.hpp"

#include <fmt/format.h>

#include <fstream>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

namespace front {
namespace intake {

group_s::group_s(
      traced_file_ptr& traced_file, 
      runtime::context_c &ctx,
      front::import_c &importer,
      bool in_repl)
    : traced_file(traced_file),
      tracer(traced_file->get_tracer()),
      engine(ctx, importer),
      generator(tracer, engine),
      atomiser(generator) {

      engine.set_print_results(in_repl);

      // Tracers may need to live beyond generators, 
      // so we give them a ref to the trace stuff
      //
      //  TODO:
      //  Change the tracer stuff so that tracers only
      //  exist during the time of consuming and first exection
      //  of code. 
      //
      //  Make the engine copy over all trace data for 
      //  long-term instructions
      //
      ctx.add_traced_file(traced_file);
      ctx.add_tracer(traced_file->get_name(), tracer);
    }
// -------------------------------------------------

uint8_t repl(settings_s& settings) {

  traced_file_ptr traced_file = front::allocate_traced_file("REPL");
  group_s ig(traced_file, settings.ctx, settings.importer, true);

  std::string line;
  size_t line_no{1};
  while (std::getline(std::cin, line)) {
    ig.atomiser.submit(line, line_no++);
  }
  return 0;
}

// -------------------------------------------------

uint8_t dir(
    settings_s& settings,
    const std::string& target) {
  fmt::print("front::intake::dir is not yet completed\n");
  return 1;
}

// -------------------------------------------------

uint8_t file(
    settings_s& settings,
    const std::string& target) {
  
  traced_file_ptr traced_file = front::allocate_traced_file(target);
  group_s ig(traced_file, settings.ctx, settings.importer);

  {
    std::filesystem::path path(target); 
    if (!std::filesystem::is_regular_file(path)) {
      fmt::print("'{}' is not a regular file\n", target);
      return false;
    }
  }

  std::ifstream in(target);
  if (!in.is_open()) {
    fmt::print("Unable to open file: {}\n", target);
    return false;
  }

  size_t line_no{1};
  std::string line;
  while(std::getline(in, line)) {
    ig.atomiser.submit(line, line_no++);
  }

  in.close();

  if (!ig.atomiser.finish()) {
    return -1;
  }

  return 0;
}

} // namespace
} // namespace
