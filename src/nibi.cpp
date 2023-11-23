#include "nibi.hpp"

#include <fmt/format.h>

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

  std::string file = "scratch.test_file";

  std::optional<parse_group_s>parse_group = atomise_file(file);
  if (!parse_group.has_value()) {
    shutdown(1,
      fmt::format(
        "No parse group returned for file: {}", file));
  }

  std::optional<parse_list_t> parse_list = analyze(std::move(*parse_group));

  if (!parse_list.has_value()) {
    shutdown(2,
      fmt::format(
        "No parse list returned for file: {}", file));
  }
  
  fmt::print("Retrieved {} parse list(s)\n", (*parse_list).size());

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
