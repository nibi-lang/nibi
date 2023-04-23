#pragma once

#include "cell.hpp"
#include "memory"

#include <string_view>

// Todo: replace with phmap::parallel_node_hash_map
//       it can be a drop-in replacement for unordered_map
#include <unordered_map>

//! \brief The environment object that will be used to store
//!        and manage the cells that are used in different scopes
class env_c {
public:
  env_c() = delete;
  ~env_c();

  //! \brief Create an environment object with parameters
  //!        to load into the environment
  //! \param parameter_list The list of parameters
  //! \param parent_env The parent environment (optional)
  env_c(cell_list_t &parameter_list, env_c *parent_env = nullptr);

  //! \brief Create an environment object without parameters
  //! \param parent_env The parent environment (optional)
  env_c(env_c *parent_env = nullptr);

  //! \brief Get a cell from the environment
  //! \param name The name of the cell
  //! \return The cell if it exists in this environment or
  //!         a parent environment. otherwise, nullptr
  //! \note Use this function to get a cell from the environment
  //!       that needs to be updated if we want to ensure it exists
  //!       in the environment or in the parent
  cell_c *get_cell(std::string_view name);

  //! \brief Get the env that a cell is in
  //! \param name The name of the cell
  //! \return The env if it exists in this environment or
  //!         a parent environment. otherwise, nullptr
  env_c *get_env(std::string_view name);

  //! \brief Set a cell in the local environment
  //!        This will not update the cell in the parent environment(s)
  //! \param name The name of the cell
  //! \param cell The cell to set
  void set_local_cell(std::string_view name, cell_c &cell);

  //! \brief Drop a cell from the environment, or parent environment(s)
  //! \param name The name of the cell
  //! \returns True if the cell was dropped, false if item not found
  bool drop_cell(std::string_view name);

private:
  //! Called by env with parents, used to register child environment
  //! \param child_env The child environment
  void register_child(env_c &child_env);

  //! \brief Called by child environment to unregister itself
  void remove_child() { child_env_ = nullptr; };

  env_c *parent_env_{nullptr};
  env_c *child_env_{nullptr};
  std::unordered_map<std::string_view, cell_c *> cell_map_;
};