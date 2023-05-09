#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "common/error.hpp"
#include "common/input.hpp"
#include "common/list.hpp"
#include "common/platform.hpp"
#include "interpreter/interpreter.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/environment.hpp"
#include "libnibi/source.hpp"

namespace {

// The object that will be used as the top level env
env_c *program_global_env{nullptr};

// Source input manager used to ensure that files are only read once
// and to generate locators for cells
source_manager_c *source_manager{nullptr};
} // namespace

void teardown() {
  global_interpreter_destroy();
  global_platform_destroy();
  delete source_manager;
  delete program_global_env;
}

void setup(std::vector<std::filesystem::path> &include_dirs) {
  program_global_env = new env_c(nullptr);
  source_manager = new source_manager_c();

  // Initialize the global platofrm object
  if (!global_platform_init(include_dirs)) {
    std::cerr << "Failed to initialize global platform" << std::endl;
    teardown();
    exit(1);
  }

  // Initialize the global interpreter object
  if (!global_interpreter_init(*program_global_env, *source_manager)) {
    std::cerr << "Failed to initialize global interpreter" << std::endl;
    teardown();
    exit(1);
  }
}

void run_from_file(const std::string &file_name) {

  // List builder that will build lists from parsed tokens
  // and pass lists to a interpreter
  list_builder_c list_builder(*global_interpreter);

  // File reader that reads file and kicks off parser/ scanner
  // that will send tokens to the list builder
  file_reader_c file_reader(list_builder, *source_manager);

  // Read the file and start the process
  file_reader.read_file(file_name);
}

void run_from_dir(const std::string &file_name, const bool run_tests) {
  std::cerr << "Running from directory not yet supported!" << std::endl;
  if (run_tests) {
    std::cout << "Run tests!" << std::endl;
    return;
  }
}

void show_help() {
  std::cout << "Usage: nibi [options] [file | directory]\n" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -h, --help          Show this help message" << std::endl;
  std::cout << "  -t, --test          Run tests" << std::endl;
  std::cout << "  -i, --include       Add include directory (`:` delimited)"
            << std::endl;
}

int main(int argc, char **argv) {

  bool run_tests{false};
  std::vector<std::string> unmatched;
  std::vector<std::filesystem::path> include_dirs;
  std::vector<std::string> args(argv + 1, argv + argc);

  for (std::size_t i = 0; i < args.size(); i++) {
    if (args[i] == "-h" || args[i] == "--help") {
      show_help();
      return 0;
    }

    if (args[i] == "-t" || args[i] == "--test") {
      run_tests = true;
    }

    if (args[i] == "-i" || args[i] == "--include") {
      if (i + 1 >= args.size()) {
        std::cout << "No include directory specified" << std::endl;
        return 1;
      }
      std::stringstream ss(args[i + 1]);
      while (std::getline(ss, args[i + 1], ':')) {
        include_dirs.push_back(args[i + 1]);
      }
      ++i;
      continue;
    }

    unmatched.push_back(args[i]);
  }

  if (unmatched.size() > 1) {
    std::cout << "Unmatched arguments:" << std::endl;
    for (auto &arg : unmatched) {
      std::cout << "  " << arg << std::endl;
    }
    return 1;
  }

  bool run_as_dir = false;
  if (std::filesystem::is_directory(args[0])) {
    run_as_dir = true;
  } else if (std::filesystem::is_regular_file(args[0])) {
    run_as_dir = false;
    if (run_tests) {
      std::cout
          << "Running tests is only supported for directory-based applications"
          << std::endl;
      return 1;
    }
  } else {
    std::cout << "Invalid file or directory: " << args[0] << std::endl;
    return 1;
  }

  // Ensure all include dirs exist
  for (auto &include_dir : include_dirs) {
    if (!std::filesystem::exists(include_dir) ||
        !std::filesystem::is_directory(include_dir)) {
      return 1;
    }
  }

  setup(include_dirs);

  (run_as_dir) ? run_from_dir(args[0], run_tests) : run_from_file(args[0]);

  teardown();

  return 0;
}
