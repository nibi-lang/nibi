#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <libnibi/nibi.hpp>

#include "arger.hpp"
#include "repl/repl.hpp"

#ifndef NIBI_BUILD_HASH
#define NIBI_BUILD_HASH "unknown"
#endif

#define CALCULATE_EXECUTION_TIME 0

#if CALCULATE_EXECUTION_TIME
#include <chrono>
#endif

using namespace nibi;

namespace {

class program_data_controller_c {
public:
  program_data_controller_c(std::vector<std::string> &args,
                            std::vector<std::filesystem::path> &include_dirs)
      : args_(args), include_dirs_(include_dirs) {
    reinit_platform();
  }

  ~program_data_controller_c() { global_platform_destroy(); }

  void reset() { reinit_platform(); }

  void add_include_dir(std::filesystem::path dir) {
    include_dirs_.push_back(dir);
  }

private:
  void reinit_platform() {
    global_platform_destroy();
    if (!global_platform_init(include_dirs_, args_)) {
      std::cerr << "Failed to initialize global platform" << std::endl;
      exit(1);
    }
  }

  std::vector<std::filesystem::path> &include_dirs_;
  std::vector<std::string> &args_;
};

std::unique_ptr<program_data_controller_c> pdc{nullptr};

} // namespace

void error_callback_function(nibi::error_c error) {

  error.draw();
  std::exit(1);
}

void run_from_file(std::filesystem::path file_name) {
  auto file_interpreter =
      interpreter_factory_c::file_interpreter(error_callback_function);
  file_interpreter->interpret_file(file_name);
  file_interpreter->indicate_complete();
}

void run_from_dir(const std::string &file_name) {
  std::filesystem::path dir_path(file_name);
  auto expected_file = dir_path / nibi::config::NIBI_APP_ENTRY_FILE_NAME;
  if (std::filesystem::exists(expected_file)) {
    run_from_file(expected_file);
    return;
  }
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

  auto info = module_factory_c::module_viewer()->get_module_info(module_name);

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

void run_each_test(std::vector<std::filesystem::path> &files) {

  if (files.size() == 0) {
    std::cout << "No test files found" << std::endl;
    return;
  }

  std::cout << "Tests files: " << files.size() << std::endl;

  for (auto &test_file : files) {
    std::cout << "Running test file: " << test_file << std::endl;
    run_from_file(test_file);
    // pdc->reset();

    std::cout << "COMPLETE\n" << std::endl;
  }

  // if we get here, all tests passed
  std::cout << "All tests passed!" << std::endl;
}

void run_local_tests_dir(std::filesystem::path &dir) {

  auto test_dir = dir / nibi::config::NIBI_MODULE_TEST_DIR;

  if (!std::filesystem::exists(test_dir)) {
    std::cout << "No tests directory found" << std::endl;
    return;
  }
  if (!std::filesystem::is_directory(test_dir)) {
    std::cout << "Suspected directory `tests` exists but is not a directory"
              << std::endl;
    return;
  }
  auto file_list = std::vector<std::filesystem::path>();
  for (auto &p : std::filesystem::directory_iterator(test_dir)) {
    if (p.is_regular_file()) {
      file_list.push_back(p.path());
    }
  }
  return run_each_test(file_list);
}

void run_tests(std::string &dir) {

  // Check if its an application first
  {
    auto fpd = std::filesystem::path(dir);
    auto app_entry = fpd / nibi::config::NIBI_APP_ENTRY_FILE_NAME;
    if (std::filesystem::exists(app_entry) &&
        std::filesystem::is_regular_file(app_entry)) {
      pdc->add_include_dir(fpd);
      return run_local_tests_dir(fpd);
    }
  }

  // Check non-installed module
  {
    auto fpd = std::filesystem::path(dir);
    try {
      fpd = std::filesystem::canonical(fpd);
    } catch (...) {
    }
    auto test_dir = fpd / nibi::config::NIBI_MODULE_FILE_NAME;
    if (std::filesystem::exists(test_dir) &&
        std::filesystem::is_regular_file(test_dir)) {
      if (fpd.has_parent_path()) {
        pdc->add_include_dir(fpd.parent_path());
      }
      pdc->add_include_dir(fpd);
      return run_local_tests_dir(fpd);
    }
  }

  // Check installed modules
  {
    auto info = module_factory_c::module_viewer()->get_module_info(dir);
    pdc->reset();

    if (info.test_files.has_value()) {
      return run_each_test(info.test_files.value());
    }
  }
}

void args_error_cb(args::errors_e error, const std::string &arg) {
  std::cout << "Error [" << args::error_to_string(error) << "] " << arg
            << std::endl;
  std::exit(1);
}

int main(int argc, char **argv) {

#if CALCULATE_EXECUTION_TIME
  auto app_start = std::chrono::high_resolution_clock::now();
#endif
  std::vector<std::filesystem::path> include_dirs;
  std::vector<std::string> args =
      std::vector<std::string>(argv + 1, argv + argc);

  std::string launch_target;

  {
    args::arger_c arger;
    arger.set_error_cb(args_error_cb);
    arger.add_flag({"-h", "--help"},   "Show help", false);
    arger.add_flag({"-v", "--version"},  "Show version information", false);
    arger.add_argument({"-t", "--test"}, "Run module tests for a single module");
    arger.add_argument({"-i", "--include"}, "Add an include directories. `:` delimited");
    arger.add_argument({"-m", "--module"}, "Display module information for given module");

    arger.parse(argc, argv);

    pdc = std::make_unique<program_data_controller_c>(args, include_dirs);

    {
      auto do_action = arger.get_arg<bool>("--help");
      if (do_action.has_value() && (*do_action)) {
        show_help();
        return 0;
      }
    }

    {
      auto do_action = arger.get_arg<bool>("--version");
      if (do_action.has_value() && (*do_action)) {
        show_version();
        return 0;
      }
    }

    {
      auto do_action = arger.get_arg<std::string>("--test");
      if (do_action.has_value() && (*do_action).size() > 0) {
        pdc->add_include_dir(std::filesystem::current_path());
        run_tests(*do_action);
        return 0;
      }
    }

    {
      auto do_action = arger.get_arg<std::string>("--module");
      if (do_action.has_value() && (*do_action).size() > 0) {
        pdc->add_include_dir(std::filesystem::current_path());
        show_module_info(*do_action);
        return 0;
      }
    }

    {
      auto do_action = arger.get_arg<std::string>("--include");
      if (do_action.has_value() && (*do_action).size() > 0) {
        std::stringstream ss(*do_action);
        std::string item;
        while (std::getline(ss, item, ':')) {
          pdc->add_include_dir(item);
        }
      }
    }

    auto unmatched = arger.get_unmatched_args();
    if (unmatched.size() >= 1) {
      launch_target = arger.get_unmatched_args()[0];
    }
  }

  if (launch_target.empty()) {
    app::start_repl();
    return 0;
  }

  bool run_as_dir = false;
  if (std::filesystem::is_directory(launch_target)) {
    run_as_dir = true;
  } else if (std::filesystem::is_regular_file(launch_target)) {
    run_as_dir = false;
  } else {
    std::cout << "Invalid file or directory: " << launch_target
              << std::endl;
    return 1;
  }

  // Ensure all include dirs exist
  for (auto &include_dir : include_dirs) {
    if (!std::filesystem::exists(include_dir) ||
        !std::filesystem::is_directory(include_dir)) {
      return 1;
    }
  }

  std::filesystem::path entry_file_path(launch_target);
  entry_file_path = std::filesystem::canonical(entry_file_path);

  if (!run_as_dir && entry_file_path.has_parent_path()) {
    entry_file_path = entry_file_path.parent_path();
  }

  pdc->add_include_dir(entry_file_path);

#if CALCULATE_EXECUTION_TIME
  auto start = std::chrono::high_resolution_clock::now();
#endif

  if (run_as_dir) {
    run_from_dir(launch_target);
  } else {
    run_from_file(launch_target);
  }

#if CALCULATE_EXECUTION_TIME
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "Execution time: " << duration.count() << "ms" << std::endl;
#endif
  return 0;
}
