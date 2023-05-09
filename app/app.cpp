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

#define CALCULATE_EXECUTION_TIME 0

#if CALCULATE_EXECUTION_TIME
#include <chrono>
#endif

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

void setup(std::vector<std::filesystem::path> &include_dirs,
           std::filesystem::path &launch_location) {
  program_global_env = new env_c(nullptr);
  source_manager = new source_manager_c();

  // Initialize the global platofrm object
  if (!global_platform_init(include_dirs, launch_location)) {
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

#if CALCULATE_EXECUTION_TIME
  auto app_start = std::chrono::high_resolution_clock::now();
#endif

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
      continue;
    }

    if (args[i] == "-i" || args[i] == "--include") {
      if (i + 1 >= args.size()) {
        std::cout << "No include directory specified" << std::endl;
        return 1;
      }
      std::stringstream ss(args[i + 1]);
      std::string item;
      while (std::getline(ss, item, ':')) {
        include_dirs.push_back(item);
      }
      ++i;
      continue;
    }

    unmatched.push_back(args[i]);
  }

  if (unmatched.empty()) {
    std::cerr << "No file or directory specified" << std::endl;
    return 1;
  }

  if (unmatched.size() > 1) {
    std::cerr << "Unmatched arguments:" << std::endl;
    for (auto &arg : unmatched) {
      std::cout << "  " << arg << std::endl;
    }
    return 1;
  }

  bool run_as_dir = false;
  if (std::filesystem::is_directory(unmatched[0])) {
    run_as_dir = true;
  } else if (std::filesystem::is_regular_file(unmatched[0])) {
    run_as_dir = false;
    if (run_tests) {
      std::cout
          << "Running tests is only supported for directory-based applications"
          << std::endl;
      return 1;
    }
  } else {
    std::cout << "Invalid file or directory: " << unmatched[0] << std::endl;
    return 1;
  }

  // Ensure all include dirs exist
  for (auto &include_dir : include_dirs) {
    if (!std::filesystem::exists(include_dir) ||
        !std::filesystem::is_directory(include_dir)) {
      return 1;
    }
  }

  std::filesystem::path entry_file_path(unmatched[0]);
  entry_file_path = std::filesystem::canonical(entry_file_path);

  if (!run_as_dir && entry_file_path.has_parent_path()) {
    entry_file_path = entry_file_path.parent_path();
  }

  setup(include_dirs, entry_file_path);

#if CALCULATE_EXECUTION_TIME
  auto start = std::chrono::high_resolution_clock::now();
#endif

  if (run_as_dir) {
    run_from_dir(unmatched[0], run_tests);
  } else {
    run_from_file(unmatched[0]);
  }

#if CALCULATE_EXECUTION_TIME
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "Execution time: " << duration.count() << "ms" << std::endl;
#endif

  teardown();

#if CALCULATE_EXECUTION_TIME
  auto app_end = std::chrono::high_resolution_clock::now();
  auto app_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      app_end - app_start);
  std::cout << "Total time: " << app_duration.count() << "ms" << std::endl;
#endif

  return 0;
}
