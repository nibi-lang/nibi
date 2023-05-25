#pragma once
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace nibi {

//! \brief An interface to pull module information
class module_viewer_if {
public:
  virtual module_info_s get_module_info(std::string &directory) = 0;
};

} // namespace nibi
