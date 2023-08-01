#pragma once

#include "libnibi/RLL/rll_wrapper.hpp"
#include "libnibi/source.hpp"
#include "ref.hpp"
#include <any>
#include <cassert>
#include <cstdint>
#include <cstring>
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
  PTR = CELL_TYPE_MIN_NUMERIC,
  U8,
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

struct dict_info_s {
  cell_dict_t data;
  dict_info_s() = default;
  dict_info_s(const dict_info_s &other) : data(other.data){};
  dict_info_s(cell_dict_t other) : data(std::move(other)){};
};

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
  list_info_s(list_types_e type, cell_list_t list)
      : type(type), list(std::move(list)) {}

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
  cell_ptr cell;
};

//! \brief Environment information that can be encoded into a cell
struct environment_info_s {
  std::string name;
  std::shared_ptr<env_c> env{nullptr};
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

#pragma pack(push, 1)

//! \brief A cell
class cell_c : public ref_counted_c {
public:
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
    environment_info_s *env;
    function_info_s *fn;
    alias_s *alias;
    dict_info_s *dict;
    list_info_s *list;
  } data{0};

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
    this->data.alias = new alias_s(alias);
  }

  cell_c(list_info_s list) : type(cell_type_e::LIST) {
    this->data.list = new list_info_s(list);
  }
  cell_c(aberrant_cell_if *acif) : type(cell_type_e::ABERRANT) {
    this->data.aberrant = acif;
  }
  cell_c(function_info_s fn) : type(cell_type_e::FUNCTION) {
    this->data.fn = new function_info_s(fn);
  }
  cell_c(environment_info_s env) : type(cell_type_e::ENVIRONMENT) {
    this->data.env = new environment_info_s(env);
  }
  cell_c(cell_dict_t dict) : type(cell_type_e::DICT) {
    this->data.dict = new dict_info_s(dict);
  }

  virtual ~cell_c();

  cell_c() = delete;
  cell_c(const cell_c &other) = delete;
  cell_c(cell_c &&other) = delete;
  cell_c &operator=(const cell_c &other) = delete;
  cell_c &operator=(cell_c &&other) = delete;

  //! \brief Attempt to convert whatever data type exists to a string
  //!        and return it
  //! \param quote_strings If true, quote strings
  //! \param flatten_complex If true, flatten complex types to just their
  //! symbols (env fn) \throws cell_access_exception_c if the cell can not
  //! access data as its
  //!         listed type
  std::string to_string(bool quote_strings = false,
                        bool flatten_complex = false);

  //! \brief Deep copy the cell
  cell_ptr clone(env_c &env);

  //! \brief Create a cell with a given type
  cell_c(cell_type_e type) : type(type) {
    // Initialize the data based on given type
    switch (type) {
    case cell_type_e::NIL:
    case cell_type_e::ABERRANT:
      this->data.ptr = nullptr;
      break;
    case cell_type_e::ENVIRONMENT:
      this->data.env = new environment_info_s();
      break;
    case cell_type_e::FUNCTION:
      this->data.fn = new function_info_s("", nullptr, function_type_e::UNSET);
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
      break;
    case cell_type_e::STRING:
    case cell_type_e::SYMBOL:
      this->data.cstr = nullptr;
      break;
    case cell_type_e::LIST:
      this->data.list = new list_info_s(list_types_e::DATA);
      break;
    case cell_type_e::CHAR:
      this->data.ch = '\0';
      break;
    case cell_type_e::ALIAS:
      this->data.alias = nullptr;
      break;
    case cell_type_e::DICT:
      this->data.dict = nullptr;
      break;
    }
  }

  void update_from(cell_c &other, env_c &env) {

    // Perform any cleanup of this cell required before updating to new data

    if (this->type == cell_type_e::ENVIRONMENT) {
      throw cell_access_exception_c(
          "Reallocating a Nibi Envrionment is an illegal operation",
          this->locator);
    }

    if (this->type == cell_type_e::STRING && this->data.cstr) {
      delete[] this->data.cstr;
    }

    if (this->type == cell_type_e::FUNCTION && this->data.fn) {
      delete this->data.fn;
    }

    if (this->type == cell_type_e::LIST && this->data.list) {
      delete this->data.list;
    }

    // Set this cell's new type

    this->type = other.type;

    // Handle specific copies

    if (other.type == cell_type_e::STRING) {
      auto size = strlen(other.data.cstr);
      this->data.cstr = new char[size + 1];
      strncpy(this->data.cstr, other.data.cstr, size);
      this->data.cstr[size] = '\0';
      return;
    }

    if (other.type == cell_type_e::FUNCTION && other.data.fn) {
      this->data.fn = new function_info_s(*other.data.fn);
      return;
    }

    if (other.type == cell_type_e::LIST && other.data.list) {
      this->data.list = new list_info_s(*(other.clone(env)->data.list));
      return;
    }

    // Handle simple copies

    this->data = other.clone(env)->data;
  }

  int64_t to_integer() {
    if (is_float()) {
      return (int64_t)this->as_double();
    }
    return this->as_integer();
  }

  int64_t &as_integer() {
    if (!is_integer()) {
      throw cell_access_exception_c(
          "Cell is not an integer: " + this->to_string(), this->locator);
    }
    return data.i64;
  }

  double to_double() {
    if (is_integer()) {
      return (double)this->as_integer();
    }
    return this->as_double();
  }

  double &as_double() {
    if (static_cast<uint8_t>(type) < CELL_TYPE_MIN_FLOAT ||
        static_cast<uint8_t>(type) > CELL_TYPE_MAX_FLOAT) {
      throw cell_access_exception_c(
          "Cell is not a floating point: " + this->to_string(), this->locator);
    }
    return data.f64;
  }

  std::string as_string() {
    if (this->type != cell_type_e::STRING &&
        this->type != cell_type_e::SYMBOL) {
      throw cell_access_exception_c("Cell is not a string", this->locator);
    }
    if (!this->data.cstr)
      return "";
    return this->data.cstr;
  }

  char *as_c_string() {
    if (this->type != cell_type_e::STRING &&
        this->type != cell_type_e::SYMBOL) {
      throw cell_access_exception_c(
          "Cell does not contain a string, or a symbol", this->locator);
    }
    return this->data.cstr;
  }

  std::string as_symbol() {
    if (this->type != cell_type_e::SYMBOL) {
      throw cell_access_exception_c("Cell is not a symbol", this->locator);
    }
    if (!this->data.cstr)
      return "";
    return this->data.cstr;
  }

  cell_list_t to_list() { return this->as_list(); }

  cell_list_t &as_list() {
    if (type != cell_type_e::LIST) {
      throw cell_access_exception_c("Cell is not a list", this->locator);
    }
    return as_list_info().list;
  }

  list_info_s to_list_info() { return this->as_list_info(); }

  list_info_s &as_list_info() {
    if (type != cell_type_e::LIST) {
      throw cell_access_exception_c("Cell is not a list", this->locator);
    }
    return *data.list;
  }

  aberrant_cell_if *as_aberrant() const {
    if (type != cell_type_e::ABERRANT) {
      throw cell_access_exception_c("Cell is not an aberrant cell",
                                    this->locator);
    }
    return data.aberrant;
  }

  function_info_s &as_function_info() {
    if (this->type != cell_type_e::FUNCTION) {
      throw cell_access_exception_c("Cell is not a function", this->locator);
    }
    return *(this->data.fn);
  }

  environment_info_s &as_environment_info() {
    if (this->type != cell_type_e::ENVIRONMENT) {
      throw cell_access_exception_c("Cell is not an environment",
                                    this->locator);
    }
    return *(this->data.env);
  }

  void *as_pointer() const {
    if (type != cell_type_e::PTR) {
      throw cell_access_exception_c("Cell does not contain a pointer",
                                    this->locator);
    }
    return data.ptr;
  }

  cell_dict_t &as_dict() {
    if (this->type != cell_type_e::DICT) {
      throw cell_access_exception_c("Cell is not a dict", this->locator);
    }
    return this->data.dict->data;
  }

  void update_string(const std::string data) {
    if (this->type != cell_type_e::STRING &&
        this->type != cell_type_e::SYMBOL) {
      throw cell_access_exception_c("Cell does not contain a string to update",
                                    this->locator);
    }
    if (this->data.cstr) {
      delete[] this->data.cstr;
    }

    this->data.cstr = new char[data.size() + 1];
    strncpy(this->data.cstr, data.data(), data.size());
    this->data.cstr[data.size()] = '\0';
  }

  char as_char() const {
    if (this->type != cell_type_e::CHAR) {
      throw cell_access_exception_c("Cell is not a char", this->locator);
    }
    return this->data.ch;
  }

  cell_ptr get_alias() const {
    if (this->type != cell_type_e::ALIAS) {
      throw cell_access_exception_c("Cell is not an alias", this->locator);
    }
    return this->data.alias->cell;
  }

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

#pragma pack(pop)

//! \brief Allocate a cell
//! \params Ctor arguments for cell
//! \note This is used to centralize allocations of cells
//!       so we can swap memory management models
constexpr auto allocate_cell = [](auto... args) -> nibi::cell_ptr {
  return new cell_c(args...);
};

} // namespace nibi
