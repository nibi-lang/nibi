#pragma once

#include "defines.hpp"
#include <string>
#include <vector>

#include "front/atomiser.hpp"
#include "middle/bytecode_generator.hpp"
#include "vm/instructions.hpp"

class nibi_c {
public:
  nibi_c(std::vector<std::string> args);

  int run();

  [[nodiscard]]
  parse_group_s prepare_file(const std::string& file);
  
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

private:

  // TODO: Put macro table here (using class fwds)
  // TODO: Put core VM here (using class fwds)
  // 

  std::vector<std::string> _args;

  [[nodiscard]]
  int execute_program(
    const bytes_t& program);

  int execute_from_file(parse_group_s);
};


static nibi_c* g_nibi{nullptr};
