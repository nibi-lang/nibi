#include "api.h"

#include <mutex>

namespace nibi {

namespace {
constexpr std::size_t API_VERSION = 0;

std::mutex aberrant_id_mutex;
uint64_t aberrant_id = 1;
} // namespace

std::size_t get_api_version() { return API_VERSION; }

uint64_t get_aberrant_id() {
  std::lock_guard<std::mutex> lock(aberrant_id_mutex);
  return ++aberrant_id;
}
} // namespace nibi
