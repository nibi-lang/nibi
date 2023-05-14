#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "libnibi/cell.hpp"
#include "libnibi/common/error.hpp"
#include "libnibi/common/input.hpp"
#include "libnibi/common/list.hpp"
#include "libnibi/common/platform.hpp"
#include "libnibi/environment.hpp"
#include "libnibi/interpreter/interpreter.hpp"
#include "libnibi/modules.hpp"
#include "libnibi/source.hpp"
#include "libnibi/version.hpp"

#ifndef NIBI_BUILD_HASH
#define NIBI_BUILD_HASH "unknown"
#endif

#define CALCULATE_EXECUTION_TIME 0

#if CALCULATE_EXECUTION_TIME
#include <chrono>
#endif

using namespace nibi;

namespace {

interpreter_c *top_level_interpreter{nullptr};

// The object that will be used as the top level env
env_c *program_global_env{nullptr};

// Source input manager used to ensure that files are only read once
// and to generate locators for cells
source_manager_c *source_manager{nullptr};
} // namespace

void teardown() {
  delete top_level_interpreter;
  global_platform_destroy();
  delete source_manager;
  delete program_global_env;
}

void setup(std::vector<std::filesystem::path> &include_dirs) {
  program_global_env = new env_c(nullptr);
  source_manager = new source_manager_c();
  top_level_interpreter =
      new interpreter_c(*program_global_env, *source_manager);

  // Initialize the global platofrm object
  if (!global_platform_init(include_dirs)) {
    std::cerr << "Failed to initialize global platform" << std::endl;
    teardown();
    exit(1);
  }
}

void run_from_file(const std::string &file_name) {

  // List builder that will build lists from parsed tokens
  // and pass lists to a interpreter
  list_builder_c list_builder(*top_level_interpreter);

  // File reader that reads file and kicks off parser/ scanner
  // that will send tokens to the list builder
  file_reader_c file_reader(list_builder, *source_manager);

  // Read the file and start the process
  file_reader.read_file(file_name);
}

void run_from_dir(const std::string &file_name) {
  std::cerr << "Running from directory not yet supported!" << std::endl;
}

void show_help() {
  std::cout << "Usage: nibi [options] [file | directory]\n" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -h, --help            Show this help message" << std::endl;
  std::cout << "  -v, --version         Show version info" << std::endl;
  std::cout << "  -m, --module <name>   Show module info" << std::endl;
  std::cout << "  -t, --test            Run tests" << std::endl;
  std::cout << "  -i, --include <dirs>  Add include directory (`:` delimited)"
            << std::endl;
}

void show_version() {
  std::cout << "libnibi version: " << LIBNIBI_VERSION << std::endl;
  std::cout << "application build hash: " << NIBI_BUILD_HASH << std::endl;
}

void show_module_info(std::string module_name) {

  auto info = modules_c(*source_manager, *top_level_interpreter)
                  .get_module_info(module_name);

  std::cout << "Description: " << std::endl;
  if (info.description.has_value()) {
    std::cout << "  " << info.description.value() << std::endl;
  } else {
    std::cout << "  <none listed>\n";
  }

  std::cout << "Version: " << std::endl;
  if (info.version.has_value()) {
    std::cout << "  " << info.version.value() << std::endl;
  } else {
    std::cout << "  <none listed>\n";
  }

  std::cout << "Authors: " << std::endl;
  if (info.authors.has_value()) {
    for (auto &author : info.authors.value()) {
      std::cout << "  " << author << std::endl;
    }
  } else {
    std::cout << "  <none listed>\n";
  }

  std::cout << "Licenses: " << std::endl;
  if (info.licenses.has_value()) {
    for (auto &license : info.licenses.value()) {
      std::cout << "  " << license << std::endl;
    }
  } else {
    std::cout << "<none listed>\n";
  }

  std::cout << "Tests files: " << std::endl;
  if (info.test_files.has_value()) {
    std::cout << "  " << info.test_files.value().size() << " files present"
              << std::endl;
  } else {
    std::cout << "  <none listed>\n";
  }
}

void run_tests(std::string &dir,
               std::vector<std::filesystem::path> &include_dirs) {

  setup(include_dirs);
  auto info =
      modules_c(*source_manager, *top_level_interpreter).get_module_info(dir);
  teardown();

  if (!info.test_files.has_value()) {
    std::cout << "No test files found" << std::endl;
    return;
  }

  std::cout << "Tests files: " << info.test_files.value().size() << std::endl;

  // Setup and tear down the environment for each test file
  // to ensure that the environment is clean for each test
  for (auto &test_file : info.test_files.value()) {

    std::cout << "Running test file: " << test_file << std::endl;
    setup(include_dirs);
    run_from_file(test_file);
    teardown();
    std::cout << "COMPLETE\n" << std::endl;
  }

  // if we get here, all tests passed
  std::cout << "All tests passed!" << std::endl;
}

int main(int argc, char **argv) {

#if CALCULATE_EXECUTION_TIME
  auto app_start = std::chrono::high_resolution_clock::now();
#endif

  std::vector<std::string> unmatched;
  std::vector<std::filesystem::path> include_dirs;
  std::vector<std::string> args(argv + 1, argv + argc);

  for (std::size_t i = 0; i < args.size(); i++) {
    if (args[i] == "-h" || args[i] == "--help") {
      show_help();
      return 0;
    }

    if (args[i] == "-v" || args[i] == "--version") {
      show_version();
      return 0;
    }

    if (args[i] == "-t" || args[i] == "--test") {
      if (i + 1 >= args.size()) {
        std::cout << "No module name specified to test" << std::endl;
        return 1;
      }
      include_dirs.push_back(std::filesystem::current_path());
      run_tests(args[i + 1], include_dirs);
      return 0;
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

    if (args[i] == "-m" || args[i] == "--module") {
      if (i + 1 >= args.size()) {
        std::cout << "No module name specified" << std::endl;
        return 1;
      }
      include_dirs.push_back(std::filesystem::current_path());
      setup(include_dirs);
      show_module_info(args[i + 1]);
      teardown();
      return 0;
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

  include_dirs.push_back(entry_file_path);

  setup(include_dirs);

#if CALCULATE_EXECUTION_TIME
  auto start = std::chrono::high_resolution_clock::now();
#endif

  if (run_as_dir) {
    run_from_dir(unmatched[0]);
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
