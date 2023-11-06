#pragma once

#include <cstdint>
#include <vector>
#include <optional>

namespace machine {
namespace tools {

namespace {
  union extract_u {
    uint64_t bin;
    double d;
  };
}

static uint64_t real_to_uint64_t(const double value)
{
  extract_u d;
  d.d = value;
  return d.bin;
}

static double real_from_uint64_t(const uint64_t value)
{
  extract_u d;
  d.bin = value;
  return d.d;
}

static inline std::vector<uint8_t> pack_2(const uint16_t value)
{
  std::vector<uint8_t> encoded_bytes;
  encoded_bytes.push_back(value >> 8);
  encoded_bytes.push_back(value);
  return encoded_bytes;
}

static inline std::vector<uint8_t> pack_4(const uint32_t value)
{
  std::vector<uint8_t> encoded_bytes;
  encoded_bytes.push_back(value >> 24);
  encoded_bytes.push_back(value >> 16);
  encoded_bytes.push_back(value >> 8);
  encoded_bytes.push_back(value);
  return encoded_bytes;
}

static inline std::vector<uint8_t> pack_8(const uint64_t value)
{
  std::vector<uint8_t> encoded_bytes;
  encoded_bytes.push_back(value >> 56);
  encoded_bytes.push_back(value >> 48);
  encoded_bytes.push_back(value >> 40);
  encoded_bytes.push_back(value >> 32);
  encoded_bytes.push_back(value >> 24);
  encoded_bytes.push_back(value >> 16);
  encoded_bytes.push_back(value >> 8);
  encoded_bytes.push_back(value);
  return encoded_bytes;
}

static inline std::optional<uint16_t> unpack_2(const std::vector<uint8_t> &data)
{
  if (data.size() != sizeof(uint16_t)) {
    return std::nullopt;
  }

  uint16_t x = static_cast<uint16_t>(data[0]) << 8;
  x |= static_cast<uint16_t>(data[1]);
  return x;
}

static inline std::optional<uint32_t> unpack_4(const std::vector<uint8_t> &data)
{
  if (data.size() != sizeof(uint32_t)) {
    return std::nullopt;
  }

  uint32_t x = static_cast<uint32_t>(data[0]) << 24;
  x |= static_cast<uint32_t>(data[1]) << 16;
  x |= static_cast<uint32_t>(data[2]) << 8;
  x |= static_cast<uint32_t>(data[3]);
  return x;
}

static inline std::optional<uint64_t> unpack_8(const std::vector<uint8_t> &data)
{
  if (data.size() != sizeof(uint64_t)) {
    return std::nullopt;
  }

  uint64_t x = static_cast<uint64_t>(data[0]) << 56;
  x |= static_cast<uint64_t>(data[1]) << 48;
  x |= static_cast<uint64_t>(data[2]) << 40;
  x |= static_cast<uint64_t>(data[3]) << 32;
  x |= static_cast<uint64_t>(data[4]) << 24;
  x |= static_cast<uint64_t>(data[5]) << 16;
  x |= static_cast<uint64_t>(data[6]) << 8;
  x |= static_cast<uint64_t>(data[7]);
  return x;
}

} // namespace
} // namespace
