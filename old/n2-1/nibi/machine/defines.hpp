#pragma once

#include <cstdint>
#include <vector>

namespace machine {

static constexpr uint32_t KIBI_BYTE = 1024;
static constexpr uint32_t MIBI_BYTE = KIBI_BYTE * 1024;
static constexpr uint32_t GIBI_BYTE = MIBI_BYTE * 1024;

using bytes_t = std::vector<uint8_t>;

} // namespace
