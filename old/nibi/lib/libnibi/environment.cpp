#include "libnibi/environment.hpp"

namespace nibi {
env_c::~env_c() {}

env_c::env_c(env_c *parent_env) : parent_env_(parent_env) {}

env_c *env_c::get_env(const std::string &name) {

  if (cell_map_.find(name) != cell_map_.end()) {
    return this;
  }

  if (parent_env_) {
    return parent_env_->get_env(name);
  }

  return nullptr;
}

cell_ptr env_c::get(const std::string &name) {
  auto it = cell_map_.find(name);
  if (it != cell_map_.end()) {
    return it->second;
  }

  if (parent_env_) {
    return parent_env_->get(name);
  }

  return nullptr;
}

bool env_c::do_set(const std::string &name, const cell_ptr &cell) {

  auto it = cell_map_.find(name);
  if (it != cell_map_.end()) {
    it->second = cell;
    return true;
  }

  if (parent_env_) {
    return parent_env_->do_set(name, cell);
  }

  return false;
}

void env_c::set(const std::string &name, const cell_ptr &cell) {
  if (!do_set(name, cell)) {
    cell_map_[name] = cell;
  }
}

bool env_c::drop(const std::string &name) {

  auto it = cell_map_.find(name);
  if (it != cell_map_.end()) {
    cell_map_.erase(it);
    return true;
  }

  if (parent_env_) {
    return parent_env_->drop(name);
  }

  return false;
}
} // namespace nibi
