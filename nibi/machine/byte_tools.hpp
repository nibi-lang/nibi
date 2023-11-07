#pragma once

#include <cstdint>
#include <vector>
#include <optional>
#include <type_traits>
#include <string>

namespace machine {
namespace tools {

//! \brief Pack some type T to a vector of bytes
template<
  typename T,
  typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
static inline std::vector<uint8_t> pack(const T value) {
  return std::vector<uint8_t>(
    (uint8_t*)&value,
    (uint8_t*)&(value) + sizeof(T));
}

//! \brief Unpack some vector of bytes to type T
template<
  typename T,
  typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
static inline std::optional<T> unpack(const std::vector<uint8_t>& data) {
  if (data.size() != sizeof(T)) {
    return std::nullopt;
  }
  T* val = (T*)(data.data());
  return {*val};
}

template<
  typename T,
  typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
static inline T quick_unpack(const std::vector<uint8_t>& data) {
  T val = (T*)(data.data());
  return val;
}





static inline std::vector<uint8_t> pack_string(const std::string& str) {
  return std::vector<uint8_t>(str.begin(), str.end());
}

static inline uint64_t real_to_uint64_t(const double& value) {
  return *reinterpret_cast<const uint64_t*>(&value);
}

static inline double real_from_uint64_t(const uint64_t& value) {
  return *reinterpret_cast<const double*>(&value);
}

} // namespace
} // namespace
