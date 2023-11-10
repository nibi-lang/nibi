#include "imports.hpp"

#include <fmt/format.h>

namespace front {
  std::optional<std::filesystem::path>
    import_c::locate_file(std::filesystem::path &file_path,
                        std::filesystem::path &imported_from) {
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

    if (std::filesystem::exists(file_path) &&
        std::filesystem::is_regular_file(file_path)) {
      return {file_path};
    }

    for (auto &include_dir : _include_dirs) {
      file_path = include_dir / file_path;
      if (std::filesystem::exists(file_path) &&
          std::filesystem::is_regular_file(file_path)) {
        return {file_path};
      }
    }
    return std::nullopt;
  }
} // namespace
