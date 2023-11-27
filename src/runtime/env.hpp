#pragma once

#include "object.hpp"

#include <set>
#include <string>

#include <map>

namespace runtime {

class env_c {
public:
  using env_map_t = std::map<std::string, object_ptr>;

  env_c() = default;

  ~env_c();

  env_c(env_c *parent_env);

  env_c *get_env(const std::string &name, bool limit_scope=false);
  
  object_ptr get(const std::string &name, bool limit_scope=false);

  void set(const std::string &name, const object_ptr &object, bool limit_scope=false);

  bool drop(const std::string &name);

  env_map_t &get_map() { return object_map_; }

private:
  inline bool do_set(const std::string &name, const object_ptr &object);
  env_c *parent_env_{nullptr};
  env_map_t object_map_;
};

} // namespace
