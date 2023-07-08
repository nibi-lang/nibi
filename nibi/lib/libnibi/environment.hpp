#pragma once

#include "cell.hpp"

#include <set>
#include <string>

#include <map>

namespace nibi {
//! \brief The environment object that will be used to store
//!        and manage the cells that are used in different scopes
class env_c {
public:
  // The current implementation has been perfomance tested
  // with the following maps via test_perfs:
  //      std::unordered_map
  //      phmap::node_hash_map
  //      std::map
  //
  // std::map was the fastest by a considerable amount
  // followed by phmap::parallel_node_hash_map
  // and then std::unordered_map
  //
  using env_map_t = std::map<std::string, cell_ptr>;

  env_c() = default;
  ~env_c();

  //! \brief Create an environment object without parameters
  //! \param parent_env The parent environment to use for searching
  //!        upper level scopes
  env_c(env_c *parent_env);

  //! \brief Get the env that a cell is in
  //! \param name The name of the cell
  //! \return The env if it exists in this environment or
  //!         a parent environment. otherwise, nullptr
  env_c *get_env(const std::string &name);

  //! \brief Get a cell from the environment
  //! \param name The name of the cell
  //! \return The cell if it exists in this environment or
  //!         a parent environment. otherwise, nullptr
  //! \note Use this function to get a cell from the environment
  //!       that needs to be updated if we want to ensure it exists
  //!       in the environment or in the parent
  cell_ptr get(const std::string &name);

  //! \brief Set a cell in the environment
  //! \param name The name of the cell
  //! \param cell The cell to set
  void set(const std::string &name, const cell_ptr &cell);

  //! \brief Drop a cell from the environment, or parent environment(s)
  //! \param name The name of the cell
  //! \returns True if the cell was dropped, false if item not found
  //! \post The cell will be erased from the environment and marked for deletion
  bool drop(const std::string &name);

  //! \brief Get the map of cells in the environment
  //! \return The map of cells in the environment
  //! \note This is meant for quick cell creation and
  //!       retrieval for specific environments
  env_map_t &get_map() { return cell_map_; }

  //! \brief Indicate that a module has been loaded
  //! \param module_name The name of the module
  void indicate_loaded_module(const std::string &module_name) {
    loaded_modules_.insert(module_name);
  }

  //! \brief Check if a module has been loaded
  //! \param module_name The name of the module
  //! \return True if the module has been loaded, false otherwise
  bool is_module_loaded(const std::string &module_name) {
    if (loaded_modules_.contains(module_name)) {
      return true;
    }

    if (parent_env_ != nullptr) {
      return parent_env_->is_module_loaded(module_name);
    }

    return false;
  }

private:
  env_c *parent_env_{nullptr};
  env_map_t cell_map_;
  std::set<std::string> loaded_modules_;

  inline bool do_set(const std::string &name, const cell_ptr &cell);
};
} // namespace nibi
