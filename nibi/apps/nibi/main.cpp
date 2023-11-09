#include "platform/system.hpp"

#include "front/lexer.hpp"
#include "front/intake.hpp"

#include "machine/defines.hpp"
#include "machine/instructions.hpp"
#include "machine/byte_tools.hpp"
#include "machine/object.hpp"

#include <vector>
#include <iostream>
#include <filesystem>

#define VERSION "0.0.0"

namespace {
}

int show_help() {

  fmt::print(R"(

  Nibi Help Page [nibi v{}]

  Command                   Description
  --------------------------------------------

  -h  --help                Show this message
  -v  --version             Show version

  --------------------------------------------

  The first argument given that is not a known Nibi
  argument will be assumed to be a file or directory
  to launch.
  Following unknown arguments will be assumed to be
  arguments for the specified target file/dir.

)", VERSION);

  return 0;
}

int show_version() {
  fmt::print("Nibi version: {}\n", VERSION);
  return 0;
}

int main(int argc, char **argv) {

  std::vector<std::string> target_args;
  std::vector<std::string> target_stdin;
  runtime::context_c runtime(target_args, target_stdin);
  
  front::intake::settings_s intake_settings {runtime};

  std::string target;
  {
    std::vector<std::string> args(argv, argv + argc);

    for(size_t i = 1; i < args.size(); i++) {

      if (args[i] == "-h" || args[i] == "--help") {
        return show_help();
      }

      if (args[i] == "-v" || args[i] == "--version") {
        return show_version();
      }

      if (target.empty()) {
        target = args[i];
        continue;
      }

      target_args.push_back(args[i]);
    }
  }

  if (!platform::stdin_is_tty()) {
    std::string x;
    while(std::cin >> x) {
      target_stdin.push_back(x);
    }
  }

  if (target.empty()) {
    return front::intake::repl(intake_settings);
  }

  if (std::filesystem::is_regular_file(target)) {
    return front::intake::file(intake_settings, target);
  }

  if (std::filesystem::is_directory(target)) {
    return front::intake::dir(intake_settings, target);
  }

  fmt::print("Given target is not a file or directory: {}\n", target);
  return 1;
}

