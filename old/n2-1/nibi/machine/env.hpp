#pragma once

#include <string>
#include <unordered_map>
#include <map>
#include <optional>

#include "machine/spinlock.hpp"
#include "machine/object.hpp"
#include <fmt/format.h>

namespace machine {

class env_c {
public:
  env_c (const env_c&) = delete;
  env_c& operator= (const env_c&) = delete;

  env_c(env_c* parent=nullptr)
    : _parent(parent){}

  [[nodiscard]] inline object_c* get(const std::string& var, bool limit_scope=false) {
    auto target = _map.find(var);
    if (target != _map.end()) {
      return &target->second;
    }
    if (_parent && !limit_scope) { 
      return _parent->get(var);
    }
    return nullptr;
  }

  [[nodiscard]] inline std::optional<object_c> get_clone(const std::string& var) {
    auto* o = get(var);
    if (!o) return {};
    return { o->clone() };
  }

  inline void insert(const std::string& var, object_c o) {
    _map[var] = o;
  }

  void dump() {
    fmt::print("Dumping env...\n");
    for(auto &[name, val] : _map) {
      fmt::print(">{}:{}\n", name, val.to_string());
    }
  }

  [[nodiscard]] inline bool exists(const std::string& var) {
    return _map.count(var);
  }

private:
  env_c* _parent{nullptr};
  std::map<std::string, object_c> _map;
};


} // namespace
