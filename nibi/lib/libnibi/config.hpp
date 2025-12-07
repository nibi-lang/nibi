#pragma once

#include <cstdint>

namespace nibi {
namespace config {
static constexpr const char *NIBI_FILE_EXTENSION = ".nibi";
static constexpr const char *NIBI_MODULE_FILE_NAME = "mod.nibi";
static constexpr const char *NIBI_MODULE_TEST_DIR = "tests";
static constexpr const char *NIBI_APP_ENTRY_FILE_NAME = "main.nibi";
static constexpr const char *NIBI_SYSTEM_CONFIG_FILE_NAME = "config.nibi";
static constexpr uint32_t NIBI_MODULE_ABERRANT_ID_SIZE = 32;

using ref_counter_underlying_type_t = std::uint32_t;
using locator_line_underlying_type_t = std::uint16_t;
using locator_column_underlying_type_t = std::uint8_t;

#ifndef NIBI_USE_CELL_POOL
#define NIBI_USE_CELL_POOL 1
#endif

#ifndef NIBI_CELL_POOL_INITIAL_BATCH_SIZE
#define NIBI_CELL_POOL_INITIAL_BATCH_SIZE 64
#endif

#ifndef NIBI_CELL_POOL_GROWTH_FACTOR
#define NIBI_CELL_POOL_GROWTH_FACTOR 2
#endif

#ifndef NIBI_CELL_POOL_COUNT
#define NIBI_CELL_POOL_COUNT 4
#endif

} // namespace config
} // namespace nibi
