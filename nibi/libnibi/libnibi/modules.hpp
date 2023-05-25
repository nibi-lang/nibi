#pragma once

#include <string>
#include <vector>

#include "libnibi/cell.hpp"
#include "libnibi/environment.hpp"
#include "libnibi/source.hpp"
#include "libnibi/common/list.hpp"

#include <filesystem>
#include <optional>
#include <set>

namespace nibi {

class modules_c {
public:
  struct module_info_s {
    std::optional<std::vector<std::string>> authors{std::nullopt};
    std::optional<std::vector<std::string>> licenses{std::nullopt};
    std::optional<std::vector<std::filesystem::path>> test_files{std::nullopt};
    std::optional<std::string> description{std::nullopt};
    std::optional<std::string> version{std::nullopt};
  };

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

  //! \brief Check if a module has already been loaded
  //! \param module_name the name of the module
  //! \returns true iff the module has been loaded already
  inline bool is_module_loaded(const std::string &module_name) const {
    return loaded_modules_.contains(module_name);
  };

private:
  std::filesystem::path get_module_path(cell_ptr &module_name);

  void load_dylib(std::string &module_name, env_c &module_env,
                  std::filesystem::path &module_path, cell_ptr &dylib_cell);

  void load_source_list(std::string &module_name, env_c &module_env,
                        std::filesystem::path &module_path,
                        cell_ptr &source_list_cell);

  void execute_post_import_actions(cell_ptr &post_list,
                                   std::filesystem::path &module_path);

  std::set<std::string> loaded_modules_;
  source_manager_c &source_manager_;
  interpreter_c &ci_;
};

} // namespace nibi
