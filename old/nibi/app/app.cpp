#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <libnibi/nibi.hpp>

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

  void disable_std() { use_std_ = false; }

  bool use_std() { return use_std_; }

  std::filesystem::path get_config_file_path() { return config_file_path_; }

  std::optional<std::string> get_repl_prelude() const {
    if (!use_std_) {
      return std::nullopt;
    }
    std::string prelude = std::string("(import \"") +
                          config_file_path_.string() + std::string("\")");
    return prelude;
  }

private:
  void reinit_platform() {
    global_platform_destroy();
    if (!global_platform_init(include_dirs_, args_)) {
      std::cerr << "Failed to initialize global platform" << std::endl;
      exit(1);
    }

    if (!use_std_) {
      return;
    }

    if (!global_platform->get_nibi_path().has_value()) {
      std::cout << "Warning: nibi path not set, but `use_std` enabled.\n"
                   "This may cause errors if the standard library is not "
                   "installed."
                << std::endl;
      disable_std();
      return;
    } else {
      add_include_dir(global_platform->get_nibi_path().value());
    }
    config_file_path_ = global_platform->get_nibi_path().value() /
                        nibi::config::NIBI_SYSTEM_CONFIG_FILE_NAME;
  }

  std::vector<std::filesystem::path> &include_dirs_;
  std::vector<std::string> &args_;
  bool use_std_{true};

  std::filesystem::path config_file_path_;
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

  // Bring in the standard library if enabled
  if (pdc->use_std()) {
    file_interpreter->interpret_file(pdc->get_config_file_path());
    file_interpreter->indicate_complete();
  }

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
  std::cout << "  -n, --no-std          Do not include standard symbols"
            << std::endl;
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
    try {
      auto info = module_factory_c::module_viewer()->get_module_info(dir);
      if (info.test_files.has_value()) {
        return run_each_test(info.test_files.value());
      }
    } catch (nibi::interpreter_c::exception_c &e) {
      std::cout << "Error: " << e.what() << std::endl;
      return;
    }
    pdc->reset();
  }
}

int main(int argc, char **argv) {

#if CALCULATE_EXECUTION_TIME
  auto app_start = std::chrono::high_resolution_clock::now();
#endif
  std::vector<std::filesystem::path> include_dirs;
  std::vector<std::string> args =
      std::vector<std::string>(argv + 1, argv + argc);

  bool use_std{true};
  std::string launch_target;
  {
    pdc = std::make_unique<program_data_controller_c>(args, include_dirs);

    std::vector<std::string> args =
        std::vector<std::string>(argv + 1, argv + argc);
    for (std::size_t i = 0; i < args.size(); i++) {

      if (args[i] == "-h" || args[i] == "--help") {
        show_help();
        return 0;
      }

      if (args[i] == "-v" || args[i] == "--version") {
        show_version();
        return 0;
      }

      if (args[i] == "-n" || args[i] == "--no-std") {
        use_std = false;
        continue;
      }

      if (args[i] == "-t" || args[i] == "--test") {
        if (i + 1 >= args.size()) {
          std::cout << "Error: Expected value for [-t | --test]" << std::endl;
          return 1;
        }
        pdc->add_include_dir(std::filesystem::current_path());
        run_tests(args[++i]);
        return 0;
      }
      if (args[i] == "-m" || args[i] == "--module") {
        if (i + 1 >= args.size()) {
          std::cout << "Error: Expected value for [-m | --module]" << std::endl;
          return 1;
        }
        pdc->add_include_dir(std::filesystem::current_path());
        show_module_info(args[++i]);
        return 0;
      }

      if (args[i] == "-i" || args[i] == "--include") {
        if (i + 1 >= args.size()) {
          std::cout << "Error: Expected value for [-i | --include]"
                    << std::endl;
          return 1;
        }
        std::stringstream ss(args[++i]);
        std::string item;
        while (std::getline(ss, item, ':')) {
          pdc->add_include_dir(item);
        }
        continue;
      }

      // Launch target already set, now we assume
      // that the rest will be arguments to the program
      if (!launch_target.empty()) {
        continue;
      }

      launch_target = args[i];
    }
  }

  if (!use_std) {
    pdc->disable_std();
  }

  if (launch_target.empty()) {

    app::repl_config_s config{pdc->get_repl_prelude()};

    app::start_repl(config);
    return 0;
  }

  bool run_as_dir = false;
  if (std::filesystem::is_directory(launch_target)) {
    run_as_dir = true;
  } else if (std::filesystem::is_regular_file(launch_target)) {
    run_as_dir = false;
  } else {
    std::cout << "Invalid file or directory: " << launch_target << std::endl;
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
