#include "platform.hpp"

#include <iostream>

namespace nibi {

platform_c *global_platform = nullptr;

platform_c::platform_c(std::vector<std::filesystem::path> &include_dirs)
    : _include_dirs(include_dirs) {

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
platform_c::locate_file(std::string &file_name) {
  std::filesystem::path file_path = file_name;
  if (file_path.has_parent_path()) {
    auto parent_path = file_path.parent_path();
    if (std::filesystem::exists(parent_path) &&
        std::filesystem::is_directory(parent_path)) {
      auto pp = std::filesystem::canonical(parent_path);
      add_include_dir(pp);
    }
  }
  if (std::filesystem::exists(file_path) &&
      std::filesystem::is_regular_file(file_path)) {
    return {file_path};
  }
  for (auto &include_dir : _include_dirs) {
    file_path = include_dir / std::filesystem::path(file_name);
    if (std::filesystem::exists(file_path) &&
        std::filesystem::is_regular_file(file_path)) {
      return {file_path};
    }
  }
  if (_nibi_path.has_value()) {
    file_path = _nibi_path.value() / std::filesystem::path(file_name);
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
  if (dir_path.has_parent_path()) {
    auto parent_path = dir_path.parent_path();
    if (std::filesystem::exists(parent_path) &&
        std::filesystem::is_directory(parent_path)) {
      auto pp = std::filesystem::canonical(parent_path);
      add_include_dir(pp);
    }
  }
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

void platform_c::add_include_dir(std::filesystem::path &include_dir) {
  for (auto &dir : _include_dirs) {
    if (dir == include_dir) {
      return;
    }
  }
  _include_dirs.push_back(include_dir);
}

bool global_platform_init(std::vector<std::filesystem::path> include_dirs) {
  if (!global_platform) {
    global_platform = new platform_c(include_dirs);
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