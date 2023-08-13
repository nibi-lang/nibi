#include "platform.hpp"

#include <iostream>

namespace nibi {

platform_c *global_platform = nullptr;

platform_c::platform_c(std::vector<std::filesystem::path> &include_dirs,
                       std::vector<std::string> &program_args)
    : _include_dirs(include_dirs), _program_args(program_args) {

  if (const char *home = std::getenv("NIBI_PATH")) {
    _nibi_path = {std::filesystem::path(home)};
  } else {

    std::filesystem::path user_home;

#if defined(__linux__) || defined(__APPLE__) || defined(__unix__)
    if (const char *unix_home = std::getenv("HOME")) {
      user_home = std::filesystem::path(unix_home);
    }

#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    if (const char *home_drive = std::getenv("HOMEDRIVE")) {
      if (const char *home_path = std::getenv("HOMEPATH")) {
        user_home = std::filesystem::path(home_drive);
        std::filesystem::path path(home_path);
        user_home /= path;
      }
    }
#else
    // Unable to detect the home env
#endif
    if (!std::filesystem::exists(user_home)) {
      return;
    }
    std::filesystem::path potential_home =
        user_home / std::filesystem::path(".nibi");
    if (std::filesystem::exists(potential_home) &&
        std::filesystem::is_regular_file(potential_home)) {
      _nibi_path = {potential_home};
    }
  }

#if defined(__linux__) || defined(__unix__)
  _platform = platform_e::LINUX;
#elif defined(__APPLE__)
  _platform = platform_e::MACOS;
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  _platform = platform_e::WINDOWS;
#else
  // Unable to detect the home env
#endif
  return;
}

std::optional<std::filesystem::path> platform_c::get_nibi_path() const {
  return _nibi_path;
}

const char *platform_c::get_platform_string() const {
  switch (_platform) {
  case platform_e::LINUX:
    return "linux";
  case platform_e::MACOS:
    return "macos";
  case platform_e::WINDOWS:
    return "windows";
  default:
    return "unknown";
  }
}

std::optional<std::filesystem::path>
platform_c::locate_file(std::filesystem::path &file_path,
                        std::filesystem::path &imported_from) {

  // Check the path that the import came from to see if it has a parent
  // directory. if it does, then we should check that directory first.
  if (imported_from.has_parent_path()) {
    auto parent_path = imported_from.parent_path();
    if (std::filesystem::exists(parent_path) &&
        std::filesystem::is_directory(parent_path)) {
      auto relative_path = std::filesystem::canonical(parent_path);
      auto first_path = relative_path / file_path;
      if (std::filesystem::exists(first_path) &&
          std::filesystem::is_regular_file(first_path)) {
        return {first_path};
      }
    }
  }
  // If its not in the parent directory, or if there is no parent then we check
  // it as-is
  if (std::filesystem::exists(file_path) &&
      std::filesystem::is_regular_file(file_path)) {
    return {file_path};
  }

  // If we still haven't found it, then we check the include directories
  for (auto &include_dir : _include_dirs) {
    file_path = include_dir / file_path;
    if (std::filesystem::exists(file_path) &&
        std::filesystem::is_regular_file(file_path)) {
      return {file_path};
    }
  }

  // If we still haven't found it, then we check the nibi path
  if (_nibi_path.has_value()) {
    file_path = _nibi_path.value() / file_path;
    if (std::filesystem::exists(file_path) &&
        std::filesystem::is_regular_file(file_path)) {
      return {file_path};
    }
  }
  return {std::nullopt};
}

std::optional<std::filesystem::path>
platform_c::locate_directory(std::string &directory_name) {
  std::filesystem::path dir_path = std::filesystem::path(directory_name);
  for (auto &include_dir : _include_dirs) {
    dir_path = include_dir / std::filesystem::path(directory_name);
    if (std::filesystem::exists(dir_path) &&
        std::filesystem::is_directory(dir_path)) {
      return {dir_path};
    }
  }
  if (_nibi_path.has_value()) {
    dir_path =
        _nibi_path.value() / "modules" / std::filesystem::path(directory_name);
    if (std::filesystem::exists(dir_path) &&
        std::filesystem::is_directory(dir_path)) {
      return {dir_path};
    }
  }
  return {std::nullopt};
}

const std::vector<std::string> platform_c::get_program_args() const {
  return _program_args;
}

bool global_platform_init(std::vector<std::filesystem::path> &include_dirs,
                          std::vector<std::string> &program_args) {
  if (!global_platform) {
    global_platform = new platform_c(include_dirs, program_args);
    return true;
  }
  return false;
}

bool global_platform_destroy() {
  if (global_platform) {
    delete global_platform;
    global_platform = nullptr;
    return true;
  }
  return false;
}

} // namespace nibi
