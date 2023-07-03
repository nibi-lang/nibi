#include "libnibi/environment.hpp"

namespace nibi {
env_c::~env_c() {}

env_c::env_c(env_c *parent_env) : parent_env_(parent_env) {}

env_c *env_c::get_env(std::string name) {

  if (cell_map_.find(name) != cell_map_.end()) {
    return this;
  }

  if (parent_env_) {
    return parent_env_->get_env(name);
  }

  return nullptr;
}

cell_ptr env_c::get(std::string name) {

  auto env = get_env(name);
  if (!env) {
    return nullptr;
  }
  return env->cell_map_[name];
}

void env_c::set_new_alloc(std::string name, cell_ptr cell) {

  auto env = get_env(name);
  if (!env) {
    env = this;
  }

  env->cell_map_[name] = cell;
}

void env_c::set(std::string name, cell_ptr const &cell) {

  auto env = get_env(name);
  if (!env) {
    env = this;
  }

  env->cell_map_[name] = cell;
}

bool env_c::drop(std::string name) {

  auto env = get_env(name);
  if (!env) {
    return false;
  }

  env->cell_map_.erase(name);

  return true;
}
} // namespace nibi
