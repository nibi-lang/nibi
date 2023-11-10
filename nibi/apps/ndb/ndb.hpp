#pragma once

#include "front/intake.hpp"
#include "front/lexer.hpp"
#include "front/parser.hpp"
#include "front/tracer.hpp"
#include "machine/engine.hpp"

namespace ndb {

class ndb_c {
public:
  ndb_c(
    std::vector<std::string> target_args,
    std::vector<std::string> target_stdin)
    : _runtime(target_args, target_stdin){}

  int execute(const std::string& target);

private:
  struct page_s {
    struct tracers_s {
      front::traced_file_ptr traced_file{nullptr};
      front::tracer_ptr tracer{nullptr};
    };
    machine::engine_c engine;
    front::parser_c parser;
    front::lexer_c lexer;
  };

  runtime::context_c _runtime;
};

} // namespace
