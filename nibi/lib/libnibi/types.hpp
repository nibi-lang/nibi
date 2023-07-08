#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "libnibi/cell.hpp"

#include <unordered_map>

namespace nibi {

class file_interpreter_if;
class line_interpreter_if;
class module_viewer_if;
class error_c;

struct module_info_s {
  std::optional<std::vector<std::string>> authors{std::nullopt};
  std::optional<std::vector<std::string>> licenses{std::nullopt};
  std::optional<std::vector<std::filesystem::path>> test_files{std::nullopt};
  std::optional<std::string> description{std::nullopt};
  std::optional<std::string> version{std::nullopt};
};

using file_interpreter_ptr = std::unique_ptr<file_interpreter_if>;
using line_interpreter_ptr = std::unique_ptr<line_interpreter_if>;
using module_viewer_ptr = std::unique_ptr<module_viewer_if>;
using error_callback_f = std::function<void(error_c)>;

// Function router was tested against test_perfs
// with the following map types:
// std::map<std::string, function_info_s>
// phmap::parallel_node_hash_map<std::string, function_info_s>
// std::unordered_map<std::string, function_info_s>
//
// std::unordered_map<std::string, function_info_s> was the fastest
using function_router_t = std::unordered_map<std::string, function_info_s>;
} // namespace nibi
