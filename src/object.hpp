#pragma once

#include "macros.hpp"

#include <cstdint>
#include <string>
#include <map>
#include <vector>

namespace nibi {

// Definitions for enum id matching that help make the code
// more clear in intent
static constexpr uint8_t OBJ_TYPE_NUMERIC_MIN = 10;
static constexpr uint8_t OBJ_TYPE_ANY_INT_MIN = OBJ_TYPE_NUMERIC_MIN;
static constexpr uint8_t OBJ_TYPE_UINT_MIN = OBJ_TYPE_ANY_INT_MIN;
static constexpr uint8_t OBJ_TYPE_UINT_MAX = OBJ_TYPE_UINT_MIN + 3;
static constexpr uint8_t OBJ_TYPE_INT_MIN = OBJ_TYPE_UINT_MAX + 1;
static constexpr uint8_t OBJ_TYPE_INT_MAX = OBJ_TYPE_INT_MIN + 3;
static constexpr uint8_t OBJ_TYPE_FLOAT_MIN = OBJ_TYPE_INT_MAX + 1;
static constexpr uint8_t OBJ_TYPE_FLOAT_MAX = OBJ_TYPE_FLOAT_MIN +1;
static constexpr uint8_t OBJ_TYPE_NUMERIC_MAX = OBJ_TYPE_FLOAT_MAX + 1;
static constexpr uint8_t OBJ_TYPE_FIRST_NON_TRIVIAL = 100;

enum class object_type_e {
  NIL = 0,
  STRING,
  CHAR,
  PTR,
  // MIN NUMERIC
  U8 = OBJ_TYPE_ANY_INT_MIN,
  U16,
  U32,
  U64 = OBJ_TYPE_UINT_MAX,
  I8 = OBJ_TYPE_INT_MIN,
  I16,
  I32,
  I64 = OBJ_TYPE_INT_MAX,
  F32 = OBJ_TYPE_FLOAT_MIN,
  F64 = OBJ_TYPE_FLOAT_MAX,
  // MAX NUMERIC
  LIST = OBJ_TYPE_FIRST_NON_TRIVIAL,
};

class base_object_c {
public:
  base_object_c(object_type_e type)
    : type(type){}
  object_type_e type{object_type_e::NIL};

  bool is_numeric() const {
    return (((uint8_t)type >= OBJ_TYPE_NUMERIC_MIN) &&
             (uint8_t)type <= OBJ_TYPE_NUMERIC_MAX);
  }
  bool is_unsigned_int() const {
    return (((uint8_t)type >= OBJ_TYPE_UINT_MIN) &&
             (uint8_t)type <= OBJ_TYPE_UINT_MAX);
  }
  bool is_signed_int() const {
    return (((uint8_t)type >= OBJ_TYPE_UINT_MIN) &&
             (uint8_t)type <= OBJ_TYPE_UINT_MAX);
  }
  bool is_int() const { return (is_signed_int() || is_unsigned_int()); }
  bool is_float() const { return type == object_type_e::F32; }
  bool is_double() const { return type == object_type_e::F64; }
  bool is_real() const { return is_double() || is_float(); }
  bool is_trivial() const {
    return (uint8_t)type < OBJ_TYPE_FIRST_NON_TRIVIAL;
  }
  bool is_non_trivial() const { return (uint8_t)type >= OBJ_TYPE_FIRST_NON_TRIVIAL; }
};

using base_object_ptr = base_object_c*;

class trivial_object_c final : base_object_c {
public:
  union {
    char* s;
    char c;
    void* ptr;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
    float f32;
    double f64;
  } data {0};

  trivial_object_c(char* s)
    : base_object_c(object_type_e::STRING) { NIBI_COPY_STRING(s, this->data.s); }
  trivial_object_c(char c)
    : base_object_c(object_type_e::CHAR) { this->data.c = c; }
  trivial_object_c(void* ptr)
    : base_object_c(object_type_e::PTR) { this->data.ptr = ptr; }
  trivial_object_c(uint8_t i)
    : base_object_c(object_type_e::U8) { this->data.u8 = i; }
  trivial_object_c(uint16_t i)
    : base_object_c(object_type_e::U16) { this->data.u16 = i; }
  trivial_object_c(uint32_t i)
    : base_object_c(object_type_e::U32) { this->data.u32 = i; }
  trivial_object_c(uint64_t i)
    : base_object_c(object_type_e::U64) { this->data.u64 = i; }
  trivial_object_c(int8_t i)
    : base_object_c(object_type_e::I8) { this->data.i8 = i; }
  trivial_object_c(int16_t i)
    : base_object_c(object_type_e::I16) { this->data.i16 = i; }
  trivial_object_c(int32_t i)
    : base_object_c(object_type_e::I32) { this->data.i32 = i; }
  trivial_object_c(int64_t i)
    : base_object_c(object_type_e::I64) { this->data.i64 = i; }
  trivial_object_c(float f)
    : base_object_c(object_type_e::F32) { this->data.f32 = f; }
  trivial_object_c(double f)
    : base_object_c(object_type_e::F64) { this->data.f64 = f; }
};

class list_object_c final : base_object_c {
public:
  std::vector<base_object_ptr> data;

  bool empty() const { return data.empty(); }
  void pop_front() { data.erase(data.begin()); }
  void push_front(base_object_ptr x) { data.insert(data.begin(), x); }
  void pop_back() { data.pop_back(); }
  void push_back(base_object_ptr x) { data.push_back(x); }
};



} // namespace
