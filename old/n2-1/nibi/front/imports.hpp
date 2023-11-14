#pragma once

#include <set>
#include <string>
#include "runtime/context.hpp"
#include <filesystem>

namespace front {

class import_c {
public:

  class intercepter_if {
  public:
    virtual bool intercept_import(
      std::filesystem::path& file_path,
      std::filesystem::path& imported_from) = 0;
  };

  import_c(runtime::context_c &ctx): _ctx(ctx) {}

  void set_interceptr(intercepter_if &intercepter) {
    _intercepter = &intercepter;
  }

  bool import_file(
      std::filesystem::path& file_path,
      std::filesystem::path& imported_from);

  void add_include_dir(std::filesystem::path& path) {
    _include_dirs.push_back(path);
  }

  void add_indclude_dirs(
    std::vector<std::filesystem::path>& paths) {
    _include_dirs.insert(
      _include_dirs.end(),
      paths.begin(),
      paths.end());
  }

  std::optional<std::filesystem::path>
    locate_file(std::filesystem::path& file_path,
                std::filesystem::path& imported_from);

private:
  std::set<std::string> _imported;
  runtime::context_c &_ctx;
  intercepter_if *_intercepter{nullptr};

  std::vector<std::filesystem::path> _include_dirs;
};

}
