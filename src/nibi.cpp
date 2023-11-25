#include "nibi.hpp"

#include <fmt/format.h>
#include "front/atom_view.hpp"

nibi_c::nibi_c(std::vector<std::string> args)
  : _args(args) {}

void nibi_c::shutdown(const int& code, const std::string& message) {

  fmt::print("{}\n", message);
  std::exit(code);
}

int nibi_c::run() {

  std::string file = "scratch.test_file";

  std::vector<uint8_t> program_data;

  program_data.reserve(
    FILE_EXEC_PREALLOC_SIZE);

  if (!front::atomize_file(file, program_data)) {
    shutdown(1,
      fmt::format(
        "Failed to atomize: {}", file));
  }

  fmt::print("{} byte(s) generated for program\n", program_data.size());

  atom_view::walker_c walker(program_data);

  while(walker.has_next()) {
    auto* v = walker.next();
    fmt::print("{}\n", atom_view::view_to_string(v, true));
  }

  return 0;
}

void nibi_c::report_error(
  const std::string& internal_origin,
  const file_error_s& error,
  bool is_fatal) {

  fmt::print("{}: {}\n", ((is_fatal) ? "Fatal error" : "Warning"), error.to_string());
  if (is_fatal) {
    shutdown(1, "Non-recoverable error");
  }
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
