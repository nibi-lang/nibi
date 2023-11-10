#include "ndb.hpp"

#include <fmt/format.h>
#include <filesystem>

namespace ndb {

int ndb_c::execute(const std::string& target) {

  fmt::print("ndb execute : {}\n", target);
  return 0;
}

} // namespace
