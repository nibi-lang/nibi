#pragma once

#include "cell.hpp"

#include <string>

// Todo: replace with phmap::parallel_node_hash_map
//       it can be a drop-in replacement for unordered_map
#include <unordered_map>

//! \brief The environment object that will be used to store
//!        and manage the cells that are used in different scopes
class env_c {
public:
  ~env_c();

  //! \brief Create an environment object without parameters
  //! \param parent_env The parent environment to use for searching
  //!        upper level scopes
  env_c(env_c *parent_env);

  //! \brief Get the env that a cell is in
  //! \param name The name of the cell
  //! \return The env if it exists in this environment or
  //!         a parent environment. otherwise, nullptr
  env_c *get_env(std::string name);

  //! \brief Get a cell from the environment
  //! \param name The name of the cell
  //! \return The cell if it exists in this environment or
  //!         a parent environment. otherwise, nullptr
  //! \note Use this function to get a cell from the environment
  //!       that needs to be updated if we want to ensure it exists
  //!       in the environment or in the parent
  cell_ptr get(std::string name);

  //! \brief Set a cell in the environment
  //! \param name The name of the cell
  //! \param cell The cell to set
  void set(std::string name, cell_ptr &cell);

  //! \brief Drop a cell from the environment, or parent environment(s)
  //! \param name The name of the cell
  //! \returns True if the cell was dropped, false if item not found
  //! \post The cell will be erased from the environment and marked for deletion
  bool drop(std::string name);

  //! \brief Get the map of cells in the environment
  //! \return The map of cells in the environment
  //! \note This is meant for quick cell creation and
  //!       retrieval for specific environments
  std::unordered_map<std::string, cell_ptr> &get_map() { return cell_map_; }

private:
  env_c *parent_env_{nullptr};
  std::unordered_map<std::string, cell_ptr> cell_map_;
};