#pragma once

#include "libnibi/RLL/rll_wrapper.hpp"
#include "libnibi/source.hpp"
#include "ref.hpp"
#include <any>
#include <cassert>
#include <cstdint>
#include <exception>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#define CELL_LIST_USE_STD_VECTOR 1

#if CELL_LIST_USE_STD_VECTOR
#include <vector>
#else
#include <deque>
#endif

namespace nibi {

#if CELL_LIST_USE_STD_VECTOR
static constexpr std::size_t CELL_VEC_RESERVE_SIZE = 1;
#endif

static constexpr uint8_t CELL_TYPE_MIN_NUMERIC = 0x01;
static constexpr uint8_t CELL_TYPE_MIN_INTEGER = CELL_TYPE_MIN_NUMERIC;
static constexpr uint8_t CELL_TYPE_MAX_INTEGER = 0x10;
static constexpr uint8_t CELL_TYPE_MAX_NUMERIC = 0xB0;
static constexpr uint8_t CELL_TYPE_MIN_FLOAT = CELL_TYPE_MAX_NUMERIC - 1;
static constexpr uint8_t CELL_TYPE_MAX_FLOAT = CELL_TYPE_MAX_NUMERIC;
static constexpr uint8_t CELL_TYPE_MAX_TRIVIAL = 0xF0;

//! \brief The type of a cell
//! \note The aberrant type is Mysterious externally defined type
//!       meant to be used by external libraries that
//!       need to store data in the cell
enum class cell_type_e : uint8_t {
  NIL = 0x00,
  U8 = CELL_TYPE_MIN_NUMERIC,
  U16,
  U32,
  U64,
  I8,
  I16,
  I32,
  I64 = CELL_TYPE_MAX_INTEGER,
  F32,
  F64 = CELL_TYPE_MAX_NUMERIC,
  CHAR,
  STRING = CELL_TYPE_MAX_TRIVIAL,
  PTR,
  LIST,
  ABERRANT,
  FUNCTION,
  SYMBOL,
  ENVIRONMENT,
  DICT,
  ALIAS
};

extern const char *cell_type_to_string(const cell_type_e type);

static std::unordered_map<std::string, cell_type_e> cell_trivial_type_tag_map =
    {{":u8", cell_type_e::U8},
     {":u16", cell_type_e::U16},
     {":u32", cell_type_e::U32},
     {":u64", cell_type_e::U64},
     {":i8", cell_type_e::I8},
     {":i16", cell_type_e::I16},
     {":i32", cell_type_e::I32},
     {":i64", cell_type_e::I64},
     {":f32", cell_type_e::F32},
     {":f64", cell_type_e::F64},
     {":str", cell_type_e::STRING},
     {":nil", cell_type_e::NIL},
     {":ptr", cell_type_e::PTR},
     {":int", cell_type_e::I64},
     {":float", cell_type_e::F64},
     {":char", cell_type_e::CHAR}

};

//! \brief The type of a function that a cell holds
enum class function_type_e {
  UNSET,                // Function type is not set
  BUILTIN_CPP_FUNCTION, // Function implemented in C++
  EXTERNAL_FUNCTION,    // Function imported from shared lib
  LAMBDA_FUNCTION,      // Function defined in source code by user
  FAUX                  // Function used to redirect keywords for special
                        // processing
};

enum class list_types_e {
  INSTRUCTION, // A single list of instruction (+ 1 2 3)
  DATA,        // A complex_data [1 2 3]
  ACCESS       // An access list {a b c}
};

// Forward declarations
class env_c;
class interpreter_c;
class cell_processor_if;
class cell_c;

//! \brief A cell pointer type
using cell_ptr = ref_counted_ptr_c<cell_c>;

//! \brief A list of cells

#if CELL_LIST_USE_STD_VECTOR
using cell_list_t = std::vector<cell_ptr>;
#else
using cell_list_t = std::deque<cell_ptr>;
#endif

//! \brief A function that takes a list of cells and an environment
using cell_fn_t =
    std::function<cell_ptr(cell_processor_if &ci, cell_list_t &, env_c &)>;

//! \brief A dictionary type
using cell_dict_t = std::unordered_map<std::string, cell_ptr>;

//! \brief Lambda information that can be encoded into a cell
struct lambda_info_s {
  std::vector<std::string> arg_names;
  cell_ptr body{nullptr};
};

//! \brief Function wrapper that holds the function
//!        pointer, the name, and the type of the function
//! \note  The operating env is a raw pointer as it may
//!        be owned by the cell or not. Lambdas for instance
//!        point to the environment they were defined in
//!        but do not own it, while MACROS own the environment
//!        to hold onto construction this->data. While two pointers
//!        or a further wrapper could be used, this is lighter
struct function_info_s {
  std::string name;
  cell_fn_t fn;
  function_type_e type;
  std::optional<lambda_info_s> lambda{std::nullopt};
  env_c *operating_env{nullptr};
  function_info_s() : name(""), fn(nullptr), type(function_type_e::UNSET){};
  function_info_s(std::string name, cell_fn_t fn, function_type_e type,
                  env_c *env = nullptr)
      : name(name), fn(fn), type(type), operating_env(env) {}
};

//! \brief List wrapper that holds list meta data
struct list_info_s {
  list_types_e type;
  cell_list_t list;
  list_info_s(list_types_e type, cell_list_t list) : type(type), list(list) {}

  list_info_s(list_types_e type) : type(type) {
#if CELL_LIST_USE_STD_VECTOR
    list.reserve(CELL_VEC_RESERVE_SIZE);
#endif
  }
};

// Temporary wrapper to distnguish strings from symbols
// in the cell constructor
struct symbol_s {
  std::string data;
};

// Temporary wrapper to distinguish aliases
struct alias_s {
  cell_ptr data;
};

//! \brief Environment information that can be encoded into a cell
struct environment_info_s {
  std::string name;
  std::shared_ptr<env_c> env{nullptr};
};

//! \brief Pointer information
struct pointer_info_s {
  bool is_owned{false};
  // In the event of getting something from ffi we wont know
  // the size. So even if we acquire ownership, we may not
  // have access to this. We keep the size for what
  // we create so we can bound check
  std::optional<std::size_t> size_bytes{std::nullopt};
};

//! \brief An exception that is thrown when a cell is accessed
//!        in a way that does not correspond to its type
class cell_access_exception_c : public std::exception {
public:
  //! \brief Create the exception
  //! \param message The message to display
  //! \param source_location The location in the source code
  cell_access_exception_c(std::string message, locator_ptr source_location)
      : message_(message), source_location_(source_location) {}

  //! \brief Get the message
  char *what() { return const_cast<char *>(message_.c_str()); }

  //! \brief Get the source location
  locator_ptr get_source_location() const { return source_location_; }

private:
  std::string message_;
  locator_ptr source_location_;
};

//! \brief A cell type that can be used to store data
//!        in the interpreter from an external dynamic library
//!
//!       This is a callback interface that the external library
class aberrant_cell_if {
public:
  //! \brief Create the cell
  aberrant_cell_if() = default;

  //! \brief Create the cell with a tag
  //! \param tag The tag to set
  aberrant_cell_if(std::size_t tag) : tag_(tag) {}

  virtual ~aberrant_cell_if() = default;
  //! \brief Convert the cell to a string
  //! \note If an exception occurs, throw a cell_access_exception_c
  //!       with the message
  virtual std::string represent_as_string() = 0;

  //! \brief Clone the cell
  virtual aberrant_cell_if *clone() = 0;

  //! \brief Get the tag of the cell
  std::size_t get_tag() const { return tag_; }

  //! \brief Check if the cell is tagged
  //! \return True if the cell is tagged
  bool is_tagged() const { return tag_ != 0; }

protected:
  //! \brief Set the tag of the cell
  //! \param tag The tag to set
  void set_tag(const std::size_t tag) {
    assert(0 != tag);
    tag_ = tag;
  }

private:
  std::size_t tag_{0};
};

//! \brief A cell
class cell_c : public ref_counted_c {
public:
  //! \brief Create a cell with a given type
  cell_c(cell_type_e type) : type(type) {
    // Initialize the data based on given type
    switch (type) {
    case cell_type_e::NIL:
    case cell_type_e::ABERRANT:
      this->data.ptr = nullptr;
      break;
    case cell_type_e::ENVIRONMENT:
      complex_data = environment_info_s{"", nullptr};
      break;
    case cell_type_e::FUNCTION:
      complex_data = function_info_s("", nullptr, function_type_e::UNSET);
      break;
    case cell_type_e::I8:
    case cell_type_e::I16:
    case cell_type_e::I32:
    case cell_type_e::I64:
    case cell_type_e::U8:
    case cell_type_e::U16:
    case cell_type_e::U32:
    case cell_type_e::U64:
      this->data.i64 = int64_t(0);
      break;
    case cell_type_e::F32:
    case cell_type_e::F64:
      this->data.f64 = double(0.00);
      break;
    case cell_type_e::PTR:
      this->data.ptr = nullptr;
      this->complex_data = pointer_info_s{false, 0};
      break;
    case cell_type_e::STRING:
    case cell_type_e::SYMBOL:
      complex_data = std::string();
      this->data.cstr = as_c_string();
      break;
    case cell_type_e::LIST:
      complex_data = list_info_s(list_types_e::DATA);
      break;
    case cell_type_e::CHAR:
      this->data.ch = '\0';
      break;
    }
  }

  cell_c(int8_t data) : type(cell_type_e::I8) { this->data.i8 = data; }
  cell_c(int16_t data) : type(cell_type_e::I16) { this->data.i16 = data; }
  cell_c(int32_t data) : type(cell_type_e::I32) { this->data.i32 = data; }
  cell_c(int64_t data) : type(cell_type_e::I64) { this->data.i64 = data; }
  cell_c(uint8_t data) : type(cell_type_e::U8) { this->data.u8 = data; }
  cell_c(uint16_t data) : type(cell_type_e::U16) { this->data.u16 = data; }
  cell_c(uint32_t data) : type(cell_type_e::U32) { this->data.u32 = data; }
  cell_c(uint64_t data) : type(cell_type_e::U64) { this->data.u64 = data; }
  cell_c(float data) : type(cell_type_e::F32) { this->data.f32 = data; }
  cell_c(double data) : type(cell_type_e::F64) { this->data.f64 = data; }
  cell_c(char data) : type(cell_type_e::CHAR) { this->data.ch = data; }
  cell_c(std::string data) : type(cell_type_e::STRING) { update_string(data); }
  cell_c(symbol_s data) : type(cell_type_e::SYMBOL) {
    update_string(data.data);
  }
  cell_c(alias_s alias) : type(cell_type_e::ALIAS) {
    this->complex_data = alias.data;
  }

  cell_c(list_info_s list) : type(cell_type_e::LIST) { complex_data = list; }
  cell_c(aberrant_cell_if *acif) : type(cell_type_e::ABERRANT) {
    this->data.aberrant = acif;
  }
  cell_c(function_info_s fn) : type(cell_type_e::FUNCTION) {
    complex_data = fn;
  }
  cell_c(environment_info_s env) : type(cell_type_e::ENVIRONMENT) {
    complex_data = env;
  }
  cell_c(cell_dict_t dict) : type(cell_type_e::DICT) { complex_data = dict; }

  cell_c() = delete;
  cell_c(const cell_c &other) = delete;
  cell_c(cell_c &&other) = delete;
  cell_c &operator=(const cell_c &other) = delete;
  cell_c &operator=(cell_c &&other) = delete;
  virtual ~cell_c();

  cell_type_e type{cell_type_e::NIL};
  locator_ptr locator{nullptr};

  union {
    void *ptr;
    char *cstr;
    char ch;
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    float f32;
    double f64;
    aberrant_cell_if *aberrant;
  } data;

  std::any complex_data{0};

  //! \brief Deep copy the cell
  cell_ptr clone(env_c &env);

  //! \brief Update the cell data and type to match another cell
  //! \param other The other cell to match
  //! \note This will not update the locator
  void update_from(cell_c &other, env_c &env);

  //! \brief Get a copy of the cell value
  //! \throws cell_access_exception_c if the cell is not an integer type
  int64_t to_integer();

  //! \brief Get a reference to the cell value
  //! \throws cell_access_exception_c if the cell is not an integer type
  int64_t &as_integer();

  //! \brief Get a copy of the cell value
  //! \throws cell_access_exception_c if the cell is not a double type
  double to_double();

  //! \brief Get a reference to the cell value
  //! \throws cell_access_exception_c if the cell is not a double type
  double &as_double();

  //! \brief Attempt to convert whatever data type exists to a string
  //!        and return it
  //! \param quote_strings If true, quote strings
  //! \param flatten_complex If true, flatten complex types to just their
  //! symbols (env fn) \throws cell_access_exception_c if the cell can not
  //! access data as its
  //!         listed type
  std::string to_string(bool quote_strings = false,
                        bool flatten_complex = false);

  //! \brief Get the string data as a reference
  //! \throws cell_access_exception_c if the cell is not a string type
  std::string &as_string();

  //! \brief Get the string data as a c string
  //! \throws cell_access_exception_c if the cell is not a string type
  char *as_c_string();

  //! \brief Get the symbol value
  //! \throws cell_access_exception_c if the cell is not a symbol type
  std::string &as_symbol();

  //! \brief Get a copy of the cell value
  //! \throws cell_access_exception_c if the cell is not a list type
  list_info_s to_list_info();

  //! \brief Get a reference to the cell value
  //! \throws cell_access_exception_c if the cell is not a list type
  list_info_s &as_list_info();

  //! \brief Get a copy of the cell value
  //! \throws cell_access_exception_c if the cell is not a list type
  cell_list_t to_list();

  //! \brief Get a reference to the cell value
  //! \throws cell_access_exception_c if the cell is not a list type
  cell_list_t &as_list();

  //! \brief Get a copy of the cell value
  //! \throws cell_access_exception_c if the cell is not an aberrant type
  aberrant_cell_if *as_aberrant() const;

  //! \brief Get a reference of the cell value
  //! \throws cell_access_exception_c if the cell is not a function type
  function_info_s &as_function_info();

  //! \brief Get a reference of the cell value
  //! \throws cell_access_exception_c if the cell is not an environment type
  environment_info_s &as_environment_info();

  //! \brief Get a reference of the cell value
  //! \throws cell_access_exception_c if the cell is not a dict type
  cell_dict_t &as_dict();

  //! \brief Get a reference to the cell c-pointer info
  //! \throws cell_access_exception_c if the cell is not a pointer type
  pointer_info_s &as_pointer_info();

  //! \brief Get the c pointer being held on to by the cell
  //! \throws cell_access_exception_c if the cell is not a pointer type
  void *as_pointer() const;

  //! \brief Update the cells string data
  //! \param str The new string data
  //! \throws cell_access_exception_c if the cell does not contain a string
  void update_string(const std::string str);

  //! \brief Return the data as a char
  //! \throws cell_access_exception_c if the cell is not a char type
  char as_char() const;

  //! \brief Retuen the aliased cell
  cell_ptr get_alias() const;

  //! \brief Check if a cell is an integer
  inline bool is_integer() const {
    return static_cast<uint8_t>(type) >= CELL_TYPE_MIN_INTEGER &&
           static_cast<uint8_t>(type) <= CELL_TYPE_MAX_INTEGER;
  }

  //! \brief Check if a cell is a floating point number
  inline bool is_float() const {
    return static_cast<uint8_t>(type) >= CELL_TYPE_MIN_FLOAT &&
           static_cast<uint8_t>(type) <= CELL_TYPE_MAX_FLOAT;
  }

  //! \brief Check if a cell is a numeric type
  inline bool is_numeric() const {
    return static_cast<uint8_t>(type) >= CELL_TYPE_MIN_NUMERIC &&
           static_cast<uint8_t>(type) <= CELL_TYPE_MAX_NUMERIC;
  }
};

//! \brief Allocate a cell
//! \params Ctor arguments for cell
//! \note This is used to centralize allocations of cells
//!       so we can swap memory management models
constexpr auto allocate_cell = [](auto... args) -> nibi::cell_ptr {
  return new cell_c(args...);
};

} // namespace nibi
