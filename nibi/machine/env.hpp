#pragma once

#include <string>
#include <unordered_map>
#include <optional>

#include "machine/spinlock.hpp"
#include "machine/object.hpp"

namespace machine {

class env_c {
public:
  env_c(env_c* parent=nullptr)
    : _parent(parent){}

  [[nodiscard]] inline object_c* get(const std::string& var) {
    auto target = _map.find(var);
    if (target != _map.end()) {
      return &target->second;
    }
    if (_parent) { return _parent->get(var); }
    return nullptr;
  }

  [[nodiscard]] inline std::optional<object_c> get_clone(const std::string& var) {
    auto* o = get(var);
    if (!o) return {};
    return { o->clone() };
  }

  inline void insert(const std::string& var, const object_c& o) {
    _map[var] = o;
  }

  [[nodiscard]] inline bool exists(const std::string& var) {
    return _map.count(var);
  }

private:
  env_c* _parent{nullptr};
  std::unordered_map<std::string, object_c> _map;
};


} // namespace
