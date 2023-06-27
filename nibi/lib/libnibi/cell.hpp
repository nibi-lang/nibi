#pragma once

#include "libnibi/RLL/rll_wrapper.hpp"
#include "libnibi/source.hpp"
#include <any>
#include <cstdint>
#include <deque>
#include <exception>
#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <string>

namespace nibi {
//! \brief The type of a cell
//! \note The aberrant type is Mysterious externally defined type
//!       meant to be used by external libraries that
//!       need to store data in the cell
enum class cell_type_e {
  NIL,
  INTEGER,
  DOUBLE,
  STRING,
  LIST,
  ABERRANT,
  FUNCTION,
  SYMBOL,
  ENVIRONMENT,
};

extern const char *cell_type_to_string(const cell_type_e type);

//! \brief The type of a function that a cell holds
enum class function_type_e {
  UNSET,                // Function type is not set
  BUILTIN_CPP_FUNCTION, // Function implemented in C++
  EXTERNAL_FUNCTION,    // Function imported from shared lib
  LAMBDA_FUNCTION,      // Function defined in source code by user
  MACRO                 // Macro
};

enum class list_types_e {
  INSTRUCTION, // A single list of instruction (+ 1 2 3)
  DATA,        // A data list [1 2 3]
  ACCESS       // An access list {a b c}
};

// Forward declarations
class env_c;
class cell_c;
class interpreter_c;
class cell_processor_if;

//! \brief A cell pointer type
using cell_ptr = std::shared_ptr<cell_c>;

constexpr auto allocate_cell = [](auto... args) -> nibi::cell_ptr {
  return std::make_shared<nibi::cell_c>(args...);
};

//! \brief A list of cells
using cell_list_t = std::deque<cell_ptr>;

//! \brief A function that takes a list of cells and an environment
using cell_fn_t =
    std::function<cell_ptr(cell_processor_if &ci, cell_list_t &, env_c &)>;

//! \brief Lambda information that can be encoded into a cell
struct lambda_info_s {
  std::vector<std::string> arg_names;
  cell_ptr body{nullptr};
};

//! \brief Function wrapper that holds the function
//!        pointer, the name, and the type of the function
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
};

// Temporary wrapper to distnguish strings from symbols
// in the cell constructor
struct symbol_s {
  std::string data;
};

//! \brief Environment information that can be encoded into a cell
struct environment_info_s {
  std::string name;
  // The environment that this cell points to,
  // may be owned, my not be so we use a raw pointer
  env_c *env{nullptr};
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
  virtual ~aberrant_cell_if() = default;
  //! \brief Convert the cell to a string
  //! \note If an exception occurs, throw a cell_access_exception_c
  //!       with the message
  virtual std::string represent_as_string() = 0;

  //! \brief Clone the cell
  virtual aberrant_cell_if *clone() = 0;
};

//! \brief A cell
class cell_c {
public:
  //! \brief Create a cell with a given type
  cell_c(cell_type_e type) : type(type) {
    // Initialize the data based on given type
    switch (type) {
    case cell_type_e::NIL:
      [[fallthrough]];
    case cell_type_e::ABERRANT:
      data = nullptr;
      break;
    case cell_type_e::ENVIRONMENT:
      data = environment_info_s{"", nullptr};
      break;
    case cell_type_e::FUNCTION:
      data = function_info_s("", nullptr, function_type_e::UNSET);
      break;
    case cell_type_e::INTEGER:
      data = int64_t(0);
      break;
    case cell_type_e::DOUBLE:
      data = double(0.00);
      break;
    case cell_type_e::STRING:
      [[fallthrough]];
    case cell_type_e::SYMBOL:
      data = std::string();
      break;
    case cell_type_e::LIST:
      data = list_info_s(list_types_e::DATA, cell_list_t());
      break;
    }
  }
  cell_c(int64_t data) : type(cell_type_e::INTEGER), data(data) {}
  cell_c(double data) : type(cell_type_e::DOUBLE), data(data) {}
  cell_c(std::string data) : type(cell_type_e::STRING), data(data) {}
  cell_c(symbol_s data) : type(cell_type_e::SYMBOL), data(data.data) {}
  cell_c(list_info_s list) : type(cell_type_e::LIST), data(list) {}
  cell_c(aberrant_cell_if *acif) : type(cell_type_e::ABERRANT), data(acif) {}
  cell_c(function_info_s fn) : type(cell_type_e::FUNCTION), data(fn) {}
  cell_c(environment_info_s env) : type(cell_type_e::ENVIRONMENT), data(env) {}

  cell_c() = delete;
  cell_c(const cell_c &other) = delete;
  cell_c(cell_c &&other) = delete;
  cell_c &operator=(const cell_c &other) = delete;
  cell_c &operator=(cell_c &&other) = delete;
  virtual ~cell_c();

  cell_type_e type{cell_type_e::NIL};
  std::any data{0};
  locator_ptr locator{nullptr};

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
  aberrant_cell_if *as_aberrant();

  //! \brief Get a reference of the cell value
  //! \throws cell_access_exception_c if the cell is not a function type
  function_info_s &as_function_info();

  //! \brief Get a reference of the cell value
  //! \throws cell_access_exception_c if the cell is not an environment type
  environment_info_s &as_environment_info();

  //! \brief Check if a cell is a numeric type
  inline bool is_numeric() const {
    return type == cell_type_e::INTEGER || type == cell_type_e::DOUBLE;
  }
};
} // namespace nibi
