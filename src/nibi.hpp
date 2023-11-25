#pragma once

#include "defines.hpp"
#include "runtime/env.hpp"
#include "runtime/core.hpp"

#include <string>
#include <vector>

class nibi_c {
public:
  nibi_c(std::vector<std::string> args);

  int run();

  [[noreturn]] 
  void shutdown(const int& code, const std::string& message);

  static std::string get_version() {
    return fmt::format(
        "version:{}.{}.{}", 
        VERSION_MAJOR,
        VERSION_MINOR,
        VERSION_PATCH);
  }

  static std::string get_build_hash() {
    return fmt::format(
        "build:{}",
        BUILD_HASH);
  }

  void report_error(
    const std::string& internal_origin,
    const file_error_s& error,
    bool is_fatal=true);

private:

  // TODO: Put macro table here (using class fwds)
  // TODO: Put core VM here (using class fwds)
  // 

  runtime::env_c _global_env;
  std::vector<std::string> _args;

  void draw(
    const std::string& file,
    const file_position_s& pos,
    const std::string& msg);
};


static nibi_c* g_nibi{nullptr};
