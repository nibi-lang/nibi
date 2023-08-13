#pragma once

namespace nibi {

// Version info
#define NIBI_MAJOR_VERSION (0)
#define NIBI_MINOR_VERSION (1)
#define NIBI_PATCH_VERSION (0)
#define NIBI_VERSION "0.1.0"

struct rt_settings_s {
  bool debug_enabled{false};
};

static rt_settings_s runtime_settings { false };

}
