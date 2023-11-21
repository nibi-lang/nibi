#include "nibi.hpp"

#include <fmt/format.h>
#include "middle/bytecode_generator.hpp"

nibi_c::nibi_c(std::vector<std::string> args)
  : _args(args) {

  // TODO: Parse args and prepare app
  // TODO: Read piped in data and store
  // TODO: Setup VM
}

void nibi_c::shutdown(const int& code, const std::string& message) {

  // TODO: Shutdown with grace.
  fmt::print("{}\n", message);
  std::exit(code);
}

int nibi_c::run() {

  // Check args for REPL / file exec / compile etc

  return execute_from_file(
    std::move(prepare_file("assign.test_file")));
}

parse_group_s nibi_c::prepare_file(const std::string& file) {

  std::optional<parse_group_s>parse_group = atomise_file(file);
  if (!parse_group.has_value()) {
    shutdown(1,
      fmt::format(
        "No parse group returned for file: {}", file));
  }

  return std::move(*parse_group);
}

int nibi_c::execute_program(const bytes_t& program) {


  // TODO: Need to make an interface for VM to callback
  //       errors on. Nibi_c should have a has-a relation
  //       ship with it - It will contain filegroup pointer
  //       for it and it will handle errors/ reconstitution

  
  DEBUG_OUT(
    fmt::format(
      "Execute a program of {} bytes", program.size()))

  return 0;
}

int nibi_c::execute_from_file(parse_group_s pg) {

  DEBUG_OUT(
    fmt::format(
      "Execute from file: {}", pg.origin));

  bytes_t program;
  program.reserve(FILE_EXEC_PREALLOC_SIZE);
  for(auto &&list : pg.lists) {
    bytes_t bytes = generate_instructions(list);

    program.insert(
      program.end(),
      bytes.begin(),
      bytes.end());
  }

  pg.lists.clear();
  program.shrink_to_fit();
  return execute_program(program);
}

// ---------------------------------------
//                  Entry
// ---------------------------------------

int main(int argc, char** argv) {


  nibi_c app(std::vector<std::string>(argv, argv + argc));

  g_nibi = &app;

  DEBUG_OUT(
    fmt::format("{} | {}",
      app.get_version(),
      app.get_build_hash()))

  return app.run();
}
