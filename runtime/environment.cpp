#include "environment.hpp"

env_c::~env_c() {
  // If there is a parent environment, remove this environment
  // from the parent environment
  if (parent_env_) {
    parent_env_->remove_child();
  }

  // Mark all cells in the environment as not being in use
  for (auto &[name, cell] : cell_map_) {
    cell->mark_as_in_use(false);
  }
}

env_c::env_c(cell_list_t &parameter_list, env_c *parent_env)
    : parent_env_(parent_env) {

  // If a parent environment is provided, register this environment
  if (parent_env_) {
    parent_env_->register_child(*this);
  }
}

env_c::env_c(env_c *parent_env) : parent_env_(parent_env) {

  // If a parent environment is provided, register this environment
  if (parent_env_) {
    parent_env_->register_child(*this);
  }
}

env_c *env_c::get_env(std::string name) {

  // Check current environment first
  if (cell_map_.find(name) != cell_map_.end()) {
    return this;
  }

  // If not found, check parent environment
  if (parent_env_) {
    return parent_env_->get_env(name);
  }

  // If not found, return nullptr
  return nullptr;
}

cell_c *env_c::get(std::string name) {
  auto *env = get_env(name);
  if (!env) {
    return nullptr;
  }
  return env->cell_map_[name];
}

void env_c::set(std::string name, cell_c &cell) {
  auto *env = get_env(name);
  if (!env) {
    env = this;
  }
  // Indicate that the cell is in use
  cell.mark_as_in_use(true);

  // Set this environment as the canary for the cell
  // in case its overwritten and lost to the environment
  // so it can mark itself as not in use
  cell.set_canary(this);

  // Store the cell
  env->cell_map_[name] = &cell;
}

bool env_c::drop(std::string name) {

  auto *env = get_env(name);
  if (!env) {
    return false;
  }

  // Immediately mark the cell as not in use
  env->cell_map_[name]->mark_as_in_use(false);

  // Remove the cell from the environment
  env->cell_map_.erase(name);

  return true;
}

void env_c::register_child(env_c &child_env) { child_env_ = &child_env; }