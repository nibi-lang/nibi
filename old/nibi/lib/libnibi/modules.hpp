#pragma once

#include <string>
#include <vector>

#include "libnibi/cell.hpp"
#include "libnibi/environment.hpp"
#include "libnibi/source.hpp"
#include "libnibi/types.hpp"

#include <filesystem>
#include <optional>
#include <unordered_map>

namespace nibi {

class modules_c {
public:
  modules_c() = delete;
  modules_c(source_manager_c &source_manager_c, interpreter_c &ci)
      : source_manager_(source_manager_c), ci_(ci) {}

  //! \brief Get information about a module
  //! \param module_name the name of the module
  module_info_s get_module_info(std::string &module_name);

  //! \brief Attempt to load a module into the given environment
  //! \param module_name the name of the module
  //! \param target_env the environment to load the module into
  //! \post If the module hasn't already been loaded it will be loaded into the
  //! target environment \note Check is_module_loaded() to see if the module has
  //! already been loaded
  void load_module(cell_ptr &module_name, env_c &target_env);

private:
  std::filesystem::path get_module_path(cell_ptr &module_name);

  void load_dylib(std::string &module_name, env_c &module_env,
                  std::filesystem::path &module_path, cell_ptr &dylib_cell);

  void load_source_list(std::string &module_name, env_c &module_env,
                        std::filesystem::path &module_path,
                        cell_ptr &source_list_cell);

  void execute_post_import_actions(cell_ptr &post_list,
                                   std::filesystem::path &module_path);

  source_manager_c &source_manager_;
  interpreter_c &ci_;
};

} // namespace nibi
