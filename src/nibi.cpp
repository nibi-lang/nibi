#include "nibi.hpp"

#include "front/front.hpp"
#include "runtime/core.hpp"
#include "runtime/runtime.hpp"
#include "runtime/builtins/builtins.hpp"
#include "rang.hpp"

#include <fmt/format.h>
#include <fstream>
#include <iomanip>
#include <vector>
#include <iostream>


nibi_c::nibi_c(std::vector<std::string> args)
  : _args(args) {
  builtins::populate_env(_global_env);
}

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
  program_data.shrink_to_fit();

  DEBUG_OUT(
    fmt::format(
      "{} byte(s) generated for program\n",
      program_data.size()));

  runtime::core_c core(file);
  auto o = core.execute(
      program_data.data(),
      program_data.size(),
      _global_env);

  if (o.get() && o->is_err()) {
    auto* e = o->as_error();
    draw(e->op, e->pos, e->message);
  }

  if (!o) return 0;

  fmt::print("({}) result: {}\n", __FILE__, o->to_string());

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

void nibi_c::draw(const std::string& file, const file_position_s& pos, const std::string& msg) {

  std::cout << rang::fg::magenta << file
            << rang::fg::reset << " : (" << rang::fg::cyan
            << pos.line << rang::fg::reset << "," << rang::fg::cyan
            << pos.col << rang::fg::reset << ")\n\n";

  struct line_data_pair_s {
    uint64_t number;
    std::string data;
  };

  std::ifstream fs;
  fs.open(file);

  if (!fs.is_open()) {
    return;
  }

  // A window of source
  std::vector<line_data_pair_s> window;

  // Get to the line
  std::string line_data;
  uint64_t line_number{0};

  // Determine the upper and lower bound for a source code window
  int64_t upper_bound = pos.line + 4;
  int64_t lower_bound = (int64_t)pos.col - 5;
  if (lower_bound < 0) {
    lower_bound = 0;
  }

  // Build a window of source code to display
  while (std::getline(fs, line_data)) {
    line_number++;
    if ((line_number >= lower_bound && lower_bound < pos.line) ||
        pos.line == line_number ||
        line_number > pos.line && line_number < upper_bound) {
      window.push_back({.number = line_number, .data = line_data});
    }

    if (line_number >= upper_bound) {
      break;
    }
  }

  fs.close();

  // Determine the alignment
  size_t width = 2;
  {
    auto s = std::to_string(upper_bound);
    if (s.length() + 1 > width) {
      width = s.length() + 1;
    }
  }

  // Make an arrow to show where the error is
  std::string pointer;
  for (size_t i = 0; i < pos.col; i++) {
    pointer += "~";
  }
  pointer += "^";

  // Draw the window
  for (auto line_data : window) {
    if (line_data.number == pos.line) {
      std::cout << rang::fg::yellow << std::right << std::setw(width)
                << line_data.number << rang::fg::reset << " | "
                << line_data.data << std::endl;
      std::cout << rang::fg::cyan << std::right << std::setw(width) << ">>"
                << rang::fg::reset << " | " << rang::fg::red << pointer
                << rang::fg::reset << std::endl;
    } else {
      std::cout << rang::fg::green << std::right << std::setw(width)
                << line_data.number << rang::fg::reset << " | "
                << line_data.data << std::endl;
    }
  }

  fmt::print("\n{}\n", msg);
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
