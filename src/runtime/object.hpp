#pragma once

#include <string>
#include <cstdint>
#include <cstring>
#include <vector>
#include <memory>
#include <functional>
#include <fmt/format.h>
#include "ref.hpp"
#include "front/atoms.hpp"

#include "runtime.hpp"

namespace runtime {

class env_c;
class object_c;

using object_ptr = ref_counted_ptr_c<object_c>;
using object_list_t = std::vector<object_ptr>;

namespace {
  constexpr size_t COMPLEX_DATA_BOUNDARY = 20;
}

enum class data_type_e : uint8_t {
  NONE,
  BOOLEAN,
  INTEGER,
  REAL,
  REF,
  STRING = COMPLEX_DATA_BOUNDARY,
  BYTES,
  IDENTIFIER,
  CPPFN,
  ERROR,
  LIST
};

extern const char* data_type_to_string(const data_type_e&);

class object_c;
using cpp_fn = std::function<runtime::object_ptr(
    const runtime::object_list_t&, runtime::env_c&)>;

class complex_object_c {
  public:
    complex_object_c() = default;
    virtual ~complex_object_c() = default;
    virtual complex_object_c* clone() = 0;
    virtual std::string to_string() = 0;
};

class object_bytes_c final : public complex_object_c {
public:
  object_bytes_c(uint8_t* in_data, const size_t& in_len) {
      this->len = in_len;
      this->data = new uint8_t[this->len];
      std::memcpy(this->data, in_data, this->len);
    }
  ~object_bytes_c() {
    if (data) {
      delete[] data;
    }
  }
  complex_object_c* clone() override { 
    return new object_bytes_c(data, len);
  }
  std::string to_string() override { 
    std::string result = "[ ";
    for(size_t i = 0; i < len; i++) {
      result += std::to_string(static_cast<int>(data[i]));
      result += " ";
    }
    result += "]";
    return result;
  }
  uint8_t* data{nullptr};
  size_t len{0};
};

class object_cpp_fn_c final : public complex_object_c {
public:
  object_cpp_fn_c(cpp_fn fn) : fn(fn) {}
  ~object_cpp_fn_c() = default;
  complex_object_c* clone() override { 
    return new object_cpp_fn_c(fn);
  }
  std::string to_string() override { return "<cppfn>"; }
  cpp_fn fn;
};

class object_list_c final : public complex_object_c {
public:
  object_list_c(const object_list_t& list) : list(list) {}
  ~object_list_c() = default;
  complex_object_c* clone() override;
  std::string to_string() override;
  object_list_t list;
};

class object_error_c final : public complex_object_c {
public:
  object_error_c(
    const std::string& op,
    const std::string& message)
    : op(op), message(message) {}
  object_error_c(
    const std::string& op,
    const std::string& message,
    const file_position_s& pos)
    : op(op), message(message), pos(pos) {}
  ~object_error_c() = default;
  complex_object_c* clone() override { 
    return new object_error_c(op, message, pos);
  }
  std::string to_string() override {
    // If a line is present, then we know we 'op' relates specifically to a file
    if (pos.line) {
      return fmt::format("Error. {}:{} message: {}", op, pos.to_string(), message);
    }
    return fmt::format("Error. op: {} message: {}", op, message);
  }
  std::string op;
  std::string message;
  file_position_s pos;
};

// Wrappers to wash away type ambiguity on 
// object constructor
struct wrap_bool_s{ const bool &data; };
struct wrap_int_s{ const int64_t &data; };
struct wrap_real_s{ const double &data; };
struct wrap_mem_ref_s{ const uint64_t &data; };
struct wrap_identifier_s{ char* data; size_t len; };

#pragma pack(push, 1)
class object_c : public ref_counted_c {
public:
  data_type_e type{data_type_e::NONE};

  static object_c boolean(bool val=false) { return object_c(wrap_bool_s{val}); }
  static object_c integer(int64_t val=0) { return object_c(wrap_int_s{val}); }
  static object_c real(double val=0.00) { return object_c(wrap_real_s{val}); }
  static object_c ref(uint64_t val=0) { return object_c(wrap_mem_ref_s{val}); }
  static object_c identifier(char* val, size_t len) { return object_c(wrap_identifier_s{val, len}); }
  static object_c str(char* val, size_t len) { return object_c(val, len); }
  static object_c fn(cpp_fn fn) { return object_c(fn); }

  object_c(){}
  object_c(const object_c &o) {
    update_from(o);
  }
  object_c(const wrap_bool_s& val)
    : type{data_type_e::BOOLEAN}
    { data.boolean = val.data; }
  object_c(const wrap_int_s& val)
    : type{data_type_e::INTEGER}
    { data.integer = val.data; }
  object_c(const wrap_real_s& val)
  : type{data_type_e::REAL}
    { data.real = val.data; }
  object_c(const wrap_mem_ref_s& val) 
    : type{data_type_e::REF}
    { data.memory_ref = val.data; }
  object_c(char* val, const size_t& len)
    : type{data_type_e::STRING}
    { setup_str(val, len); }
  object_c(uint8_t* val, const size_t& len)
    : type{data_type_e::BYTES}
    { data.co = new object_bytes_c(val, len); }
  object_c(object_error_c err)
    : type{data_type_e::ERROR}
    { data.co = err.clone(); }
  object_c(const wrap_identifier_s& val)
    : type{data_type_e::IDENTIFIER}
    { setup_str(val.data, val.len); }
  object_c(cpp_fn &fn)
    : type{data_type_e::CPPFN}
    {
      data.co = new object_cpp_fn_c(fn);
    }
  object_c(const object_list_t& list)
    : type{data_type_e::LIST}
    {
      data.co = new object_list_c(list);
    }

  ~object_c() { clean(); }

  [[nodiscard]] bool conditional_self_load(env_c* env, bool limit_scope=false);

  object_c clone() const {
    object_c o;
    o.type = this->type;
    if (this->heap_allocated())
      o.data.co = this->data.co->clone();
    else
      o.data = this->data;
    return o;
  }

  void update_from(const object_c& o) {
    this->clean();
    this->type = o.type;
    if (o.heap_allocated())
      this->data.co = o.data.co->clone();
    else
      this->data = o.data;
  }

  bool is_bool() const {
    return type == data_type_e::BOOLEAN;
  }
  bool is_integer() const {
    return type == data_type_e::INTEGER;
  }
  bool is_real() const {
    return type == data_type_e::REAL;
  }
  bool is_str() const {
    return type == data_type_e::STRING ||
           type == data_type_e::IDENTIFIER;
  }
  bool is_bytes() const {
    return type == data_type_e::BYTES;
  }
  bool is_ref() const {
    return type == data_type_e::REF;
  }
  bool is_err() const {
    return type == data_type_e::ERROR;
  }
  bool is_cpp_fn() const {
    return type == data_type_e::CPPFN;
  }
  bool is_ptr_type() const {
    return ((uint8_t)type >= COMPLEX_DATA_BOUNDARY);
  }
  bool heap_allocated() const {
    if (!is_ptr_type()) {
      return false;
    }
    return (data.co != nullptr);
  }
  bool is_numeric() const {
    return is_bool() || is_integer() || is_real();
  }
  bool is_identifier() const {
    return type == data_type_e::IDENTIFIER;
  }
  bool is_list() const {
    return type == data_type_e::LIST;
  }
  char* as_raw_str(bool& okay) {
    okay = false;
    if (!is_str()) { return nullptr; }
    if (data.co == nullptr) { return nullptr; }
    okay = true;
    return (char*)(reinterpret_cast<object_bytes_c*>(data.co)->data);
  }
  object_cpp_fn_c* as_cpp_fn() {
    return reinterpret_cast<object_cpp_fn_c*>(data.co);
  }

  std::string to_string() const;
  std::string dump_to_string(bool simple=false) const;

  // These methods are not safe on their own.
  // It is up to the caller to ensure the correct
  // type is being accessed, otherwise we get UB
  bool& as_bool() { return data.boolean; }
  int64_t& as_integer() { return data.integer; }
  double& as_real() { return data.real; }
  uint64_t& as_ref() { return data.memory_ref; }
  object_bytes_c* as_bytes() { return reinterpret_cast<object_bytes_c*>(data.co); }
  object_error_c* as_error() { return reinterpret_cast<object_error_c*>(data.co); }
  object_list_c* as_list() { return reinterpret_cast<object_list_c*>(data.co); }

  double to_real() const {
    if (is_real()) { return data.real; }
    if (is_integer()) { return (double) data.integer; }
    else if (is_bool()) { return (double) data.boolean; }
    return 0.00;
  }

  int64_t to_integer() const {
    if (is_integer()) { return data.integer; }
    if (is_real()) { return (int64_t) data.real; }
    else if (is_bool()) { return (int64_t) data.boolean; }
    return 0;
  }

#define STR1(x)  #x
 #define STR(x)  STR1(x)
#define MACHINE_OBJECT_C_OPERATION(math_op) \
    if (!is_numeric() || !o.is_numeric()) { \
      return object_c(\
          object_error_c(STR(math_op),\
            fmt::format("Invalid operation of type {} and {} (requires numeric types)", \
              data_type_to_string(type), data_type_to_string(o.type)))); \
    } \
    if (o.is_real() || is_real()) { \
      return real(to_real() math_op o.to_real());\
    }\
    return integer(to_integer() math_op o.to_integer());\

  object_c& operator= (const object_c &o) {
    update_from(o);
    return *this;
  }
  object_c operator+ (const object_c& o) {
    MACHINE_OBJECT_C_OPERATION(+)
  }
  object_c operator- (const object_c& o) {
    MACHINE_OBJECT_C_OPERATION(-)
  }
  object_c operator/ (const object_c& o) {
    if (o.to_integer() == 0) { 
      return object_c(object_error_c(
            "Attempt to divide by zero",
            "division"));
    }
    MACHINE_OBJECT_C_OPERATION(/)
  }
  object_c operator* (const object_c& o) {
    MACHINE_OBJECT_C_OPERATION(*)
  }
  object_c operator% (const object_c& o) {
    if (!is_integer() && !o.is_integer()) {
      return object_c(object_error_c(
            "Attempt to perform modulous on non-integer object",
            "modulous"));
    }
    return integer(data.integer % o.data.integer);
  }
  object_c equals(const object_c& o) {
    if (is_numeric() && o.is_numeric()) {
      if (is_real() || o.is_real()) {
        return boolean(to_real() == o.to_real());
      }
      return boolean(to_integer() == o.to_integer());
    }
    if (is_str() && o.is_str()) {
      return boolean(to_string() == o.to_string());
    }
    return object_c(
        object_error_c("eq", 
          fmt::format("Invalid comparison of type {} and {}",
            data_type_to_string(type), data_type_to_string(o.type))));
  }
  object_c not_equals(const object_c& o) {
    return boolean((equals(o).to_integer() == 0));
  }
  object_c is_less_than(const object_c& o) {
    if (is_numeric() && o.is_numeric()) {
      if (is_real() || o.is_real()) {
        return boolean(to_real() < o.to_real());
      }
      return boolean(to_integer() < o.to_integer());
    }
    return object_c(
        object_error_c("<", 
          fmt::format("Invalid comparison of type {} and {}",
            data_type_to_string(type), data_type_to_string(o.type))));
  }
  object_c is_greater_than(const object_c& o) {
    if (is_numeric() && o.is_numeric()) {
      if (is_real() || o.is_real()) {
        return boolean(to_real() > o.to_real());
      }
      return boolean(to_integer() > o.to_integer());
    }
    return object_c(
        object_error_c(">", 
          fmt::format("Invalid comparison of type {} and {}",
            data_type_to_string(type), data_type_to_string(o.type))));
  }
private:
  union {
    bool boolean;
    int64_t integer;
    double real;
    uint64_t memory_ref;
    complex_object_c* co;
  } data{0};

  inline void setup_str(char* val, size_t len) {
    data.co = new object_bytes_c((uint8_t*)val, len);
  }

  void clean() {
    if (this->heap_allocated()) { delete this->data.co; }
    this->type = data_type_e::NONE;
    data.integer = 0;
  }
};
#pragma pack(pop)

constexpr auto allocate_object = [](auto... args) -> runtime::object_ptr {
  return new object_c(args...);
};
} // namespace
