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

} // namespace config
} // namespace nibi
