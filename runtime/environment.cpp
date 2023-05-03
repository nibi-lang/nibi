#include "environment.hpp"

env_c::~env_c() {
  // If there is a parent environment, remove this environment
  // from the parent environment
  if (parent_env_) {
    parent_env_->remove_child();
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

cell_ptr env_c::get(std::string name) {
  auto env = get_env(name);
  if (!env) {
    return nullptr;
  }
  return env->cell_map_[name];
}

void env_c::set(std::string name, cell_ptr &cell) {
  auto env = get_env(name);
  if (!env) {
    env = this;
  }

  // Store the cell
  env->cell_map_[name] = cell;
}

bool env_c::drop(std::string name) {

  auto env = get_env(name);
  if (!env) {
    return false;
  }

  // Remove the cell from the environment
  env->cell_map_.erase(name);

  return true;
}

void env_c::register_child(env_c &child_env) { child_env_ = &child_env; }