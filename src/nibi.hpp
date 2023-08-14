#pragma once

#include <atomic>
#include "source.hpp"

namespace nibi {

// Version info
#define NIBI_MAJOR_VERSION (0)
#define NIBI_MINOR_VERSION (1)
#define NIBI_PATCH_VERSION (0)
#define NIBI_VERSION "0.1.0"

struct rt_settings_s {
  bool debug_enabled{false};
};

static rt_settings_s global_runtime_settings;

struct rt_flags_s {
  std::atomic<bool> operational{false};
};

static rt_flags_s global_runtime_flags;

extern void shutdown_nibi();

enum class error_origin_e {
  LEXER,
  PARSER,
  VM,
};

extern void fatal_error(
    const error_origin_e error_origin,
    locator_ptr locator,
    const std::string &msg);

}
