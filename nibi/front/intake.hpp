#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "lexer.hpp"
#include "parser.hpp"
#include "tracer.hpp"
#include "machine/engine.hpp"
#include "runtime/context.hpp"
#include "front/imports.hpp"

namespace front {
namespace intake {

struct group_s {
  traced_file_ptr traced_file{nullptr};
  tracer_ptr tracer{nullptr};

  machine::engine_c engine;
  parser_c parser;
  lexer_c lexer;

  group_s(
    traced_file_ptr& traced_file, 
    runtime::context_c &ctx,
    front::import_c &importer,
    bool in_repl=false);
};

struct settings_s {
  runtime::context_c &ctx;
  front::import_c &importer;

  settings_s(runtime::context_c &ctx, front::import_c& importer)
    : ctx(ctx), importer(importer) {}
};

extern uint8_t repl(
  settings_s& settings_s);

extern uint8_t file(
  settings_s& settings_s,
  const std::string&); 

extern uint8_t dir(
  settings_s& settings_s,
  const std::string&);

} // namespace
} // namespace
