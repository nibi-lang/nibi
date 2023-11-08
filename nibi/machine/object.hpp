#pragma once

#include <string>
#include <cstdint>
#include <cstring>
#include <vector>
#include <memory>
#include <fmt/format.h>

namespace machine {

enum class data_type_e {
  NONE,
  BOOLEAN,
  INTEGER,
  REAL,
  STRING,
  BYTES,
  REF,
  IDENTIFIER,
  ERROR
};

extern const char* data_type_to_string(const data_type_e&);

struct object_meta_data_s {
  size_t bytecode_origin_id{0};
  object_meta_data_s* clone() {
    return new object_meta_data_s {
      bytecode_origin_id
    };
  }
  std::string to_string() const;
};

struct object_byte_data_s {
  object_byte_data_s() = delete;
  object_byte_data_s(uint8_t* in_data, const size_t& in_len) {
    if (in_data == nullptr || in_len == 0) { return; }
    data = new uint8_t[in_len];
    std::memcpy(data, in_data, in_len);
    this->len = in_len;

    fmt::print("Construct new byte data of len {}\n", len);
    for(auto i = 0; i < len; i++) {
      fmt::print("{} ", (char)data[i]);
    }
    fmt::print("\n");
  }
  ~object_byte_data_s() {
    if (data) {
      delete[] data;
    }
  }
  object_byte_data_s* clone() {

    fmt::print("Clone new byte data of len {}\n", len);
    return new object_byte_data_s(data, len);
  }
  std::string to_string() const;
  uint8_t* data{nullptr};
  size_t len{0};
};

struct object_error_data_s {
  std::string message;
  std::string operation;

  object_error_data_s(
    const std::string& msg,
    const std::string& op)
    : message(msg),
      operation(op){}
  object_error_data_s* clone() const {
    return new object_error_data_s{message, operation};
  }
  std::string to_string() const;
};

// Wrappers to wash away type ambiguity on 
// object constructor
struct wrap_bool_s{ bool &data; };
struct wrap_int_s{ int64_t &data; };
struct wrap_real_s{ double &data; };
struct wrap_mem_ref_s{ uint64_t &data; };
struct wrap_identifier_s{ char* data; size_t len; };

#pragma pack(push, 1)
class object_c {
public:
  object_meta_data_s* meta{nullptr}; 
  data_type_e type{data_type_e::NONE};

  static object_c boolean(bool val=false) { return object_c(wrap_bool_s{val}); }
  static object_c integer(int64_t val=0) { return object_c(wrap_int_s{val}); }
  static object_c real(double val=0.00) { return object_c(wrap_real_s{val}); }
  static object_c ref(uint64_t val=0) { return object_c(wrap_mem_ref_s{val}); }
  static object_c identifier(char* val, size_t len) { return object_c(wrap_identifier_s{val, len}); }
  static object_c str(char* val, size_t len) { return object_c(val, len); }

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
    { data.str = new object_byte_data_s(val, len); }
  object_c(const object_error_data_s& err)
    : type{data_type_e::ERROR}
    { data.err = err.clone(); }
  object_c(const wrap_identifier_s& val)
    : type{data_type_e::IDENTIFIER}
    { setup_str(val.data, val.len); }

  ~object_c() { clean(); }

  object_c clone() const {
    object_c o;
    o.type = this->type;

    if (this->meta) o.meta = this->meta->clone();
    if (is_str()) { o.data.str = this->data.str->clone(); }
    else if (is_bytes()) {
      o.data.bytes = this->data.bytes->clone(); }
    else
      o.data = this->data;
    return o;
  }

  void update_from(const object_c& o) {
    

    fmt::print("Updating object to : {}\n", data_type_to_string(o.type));

    this->clean();
    this->type = o.type;
    if (o.meta) {
      if (this->meta) delete this->meta;
      this->meta = o.meta->clone();
    }
    if (o.is_str()) {
      fmt::print("0) Other type {} Other string {}\n\n",
          data_type_to_string(o.type),
          (char*)o.data.str->data);

      this->data.str = o.data.str->clone();

   //   fmt::print("1) Other string {}\nThis string {}\n\n",
   //       (char*)o.data.str, (char*)this->data.str);
    }
    else if (o.is_bytes()) {
      this->data.bytes = o.data.bytes->clone(); }
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
  bool is_ptr_type() const {
    return (is_str() || is_bytes() || is_err());
  }
  bool req_free() const {
    if (!is_ptr_type()) {
      return false;
    }
    return (data.bytes != nullptr);
  }
  bool is_numeric() const {
    return is_bool() || is_integer() || is_real();
  }
  bool is_identifier() const {
    return type == data_type_e::IDENTIFIER;
  }
  char* as_raw_str(bool& okay) {
    okay = false;
    if (!is_str()) { return nullptr; }
    if (data.str->data == nullptr) { return nullptr; }
    okay = true;
    return (char*)data.str->data;
  }

  std::string to_string() const;
  std::string dump_to_string() const;

  // These methods are not safe on their own.
  // It is up to the caller to ensure the correct
  // type is being accessed, otherwise we get UB
  bool& as_bool() { return data.boolean; }
  int64_t& as_integer() { return data.integer; }
  double& as_real() { return data.real; }
  uint64_t& as_ref() { return data.memory_ref; }
  object_byte_data_s* as_bytes() { return data.bytes; }

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

#define MACHINE_OBJECT_C_OPERATION(math_op) \
    if (!is_numeric() || !o.is_numeric()) { return object_c(); } \
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
      return object_c(object_error_data_s(
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
      return object_c(object_error_data_s(
            "Attempt to perform modulous on non-integer object",
            "modulous"));
    }
    return integer(data.integer % o.data.integer);
  }

private:
  union {
    bool boolean;
    int64_t integer;
    double real;
    uint64_t memory_ref;
    object_byte_data_s* str;
    object_byte_data_s* bytes;
    object_error_data_s* err;
  } data{0};

  void setup_str(char* val, size_t len) {
    data.str = new object_byte_data_s((uint8_t*)val, len);

    fmt::print("Just setup string: {}\n", (char*)data.str->data);
  }

  void clean() {
    if (is_err() && data.err != nullptr) {
      delete data.err;
    }
    else if ((is_str() || is_bytes()) && data.bytes != nullptr) {
      delete data.str;
    }
    delete this->meta;
    this->type = data_type_e::NONE;
    data.integer = 0;
  }
};
#pragma pack(pop)

using object_ptr = std::shared_ptr<object_c>;

constexpr auto allocate_object = [](auto... args) -> object_ptr {
  return std::make_shared<object_c>(args...);
};

} // namespace
