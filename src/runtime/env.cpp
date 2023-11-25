#include "env.hpp"

namespace runtime {

env_c::~env_c() {}

env_c::env_c(env_c *parent_env) : parent_env_(parent_env) {}

env_c *env_c::get_env(const std::string &name) {

  if (object_map_.find(name) != object_map_.end()) {
    return this;
  }

  if (parent_env_) {
    return parent_env_->get_env(name);
  }

  return nullptr;
}

object_ptr env_c::get(const std::string &name, bool limit_scope) {
  auto it = object_map_.find(name);
  if (it != object_map_.end()) {
    return it->second;
  }

  if (parent_env_ && !limit_scope) {
    return parent_env_->get(name);
  }

  return nullptr;
}

bool env_c::do_set(const std::string &name, const object_ptr &object) {

  auto it = object_map_.find(name);
  if (it != object_map_.end()) {
    it->second = object;
    return true;
  }

  if (parent_env_) {
    return parent_env_->do_set(name, object);
  }

  return false;
}

void env_c::set(const std::string &name, const object_ptr &object) {
  if (!do_set(name, object)) {
    object_map_[name] = object;
  }
}

bool env_c::drop(const std::string &name) {

  auto it = object_map_.find(name);
  if (it != object_map_.end()) {
    object_map_.erase(it);
    return true;
  }

  if (parent_env_) {
    return parent_env_->drop(name);
  }

  return false;
}

} // namespace
