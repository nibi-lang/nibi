#include "imports.hpp"
#include "front/intake.hpp"
#include <fmt/format.h>

namespace front {

  bool import_c::import_file(
      std::filesystem::path& file_path,
      std::filesystem::path& imported_from) {

    auto path = locate_file(file_path, imported_from);

    if (!path.has_value()) { return false; }

    std::string path_as_str = (*path).string();
    if (_imported.contains(path_as_str)) { return true; }

    if (_intercepter) {
      return _intercepter->intercept_import(file_path, imported_from);
    }

    front::intake::settings_s settings(_ctx, *this);
    
    _imported.insert(path_as_str);
    if (0 != front::intake::file(settings, path_as_str)) {
      _imported.erase(path_as_str);
      return false;
    }
    return true;
  }

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
