#pragma once

#include "atoms.hpp"
#include <functional>
#include <map>
#include <memory>
#include <string>

struct list_veify_info_s {
  const std::string& origin;
  atom_list_t& list;
};

using ptree_map_t = std::map<
  std::string,
  std::function<void(const list_veify_info_s&)>>;

// If the given list is a builtin, this method
// will recursively iterate the list to ensure that
// the syntactic structure of the builtin is valid
//
extern void verify_list(const list_veify_info_s& tbis);
