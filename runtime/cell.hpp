#pragma once

#include "common/source.hpp"
#include "memory.hpp"
#include <any>
#include <cstdint>
#include <exception>
#include <functional>
#include <list>
#include <string>

#include <iostream>

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
  REFERENCE,
  ABERRANT,
  FUNCTION,
  SYMBOL,
};

extern const char *cell_type_to_string(const cell_type_e type);

//! \brief The type of a function that a cell holds
enum class function_type_e {
  UNSET,                // Function type is not set
  BUILTIN_CPP_FUNCTION, // Function implemented in C++ and compiled with tokra
  EXTERNAL_FUNCTION,    // Function imported from shared lib
  LAMBDA_FUNCTION       // Function defined in source code by user
};

enum class list_types_e {
  INSTRUCTION, // A single list of instruction (+ 1 2 3)
  DATA         // A data list [1 2 3]
};

// Forward declarations
class env_c;
class cell_c;

//! \brief A list of cells
using cell_list_t = std::list<cell_c *>;

//! \brief A function that takes a list of cells and an environment
using cell_fn_t = std::function<cell_c *(cell_list_t &, env_c &)>;

//! \brief A cell specific allocator
using cell_memory_manager_t = memory::controller_c<cell_c>;

//! \brief A global cell representing nil
extern cell_c *global_cell_nil;

//! \brief A global cell representing true
extern cell_c *global_cell_true;

//! \brief A global cell representing false
extern cell_c *global_cell_false;

//! \brief Initialize the global cells
//! \return True if the initialization was successful
extern bool global_cells_initialize();

//! \brief Destroy the global cells
extern void global_cells_destroy();

//! \brief Function wrapper that holds the function
//!        pointer, the name, and the type of the function
struct function_info_s {
  std::string name;
  cell_fn_t fn;
  function_type_e type;
  function_info_s() : name(""), fn(nullptr), type(function_type_e::UNSET){};
  function_info_s(std::string name, cell_fn_t fn, function_type_e type)
      : name(name), fn(fn), type(type) {}
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

//! \brief A canary interface for something that manages if a cell is
//!        in use. This is used to detect memory leaks.
//! \note The idea is that a cell is given a canary and if the canary
//!       is a nullptr then the thing that owned it has been destroyed
//!       and the cell is orphaned, needing to be destroyed as well.
class cell_canary_if {
public:
  cell_canary_if() = default;
  virtual ~cell_canary_if() = default;
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
//!        in the runtime from an external dynamic library
//!
//!       This is a callback interface that the external library
class aberrant_cell_if {
public:
  virtual ~aberrant_cell_if() = default;
  //! \brief Convert the cell to a string
  //! \note If an exception occurs, throw a cell_access_exception_c
  //!       with the message
  virtual std::string represent_as_string() = 0;
};

//! \brief A cell
class cell_c final : public memory::markable_if {
public:
  //! \brief Create a cell with a given type
  cell_c(cell_type_e type) : type(type) {
    // Initialize the data based on given type
    switch (type) {
    case cell_type_e::NIL:
      [[fallthrough]];
    case cell_type_e::ABERRANT:
      [[fallthrough]];
    case cell_type_e::REFERENCE:
      data = nullptr;
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
  cell_c(cell_c *data) : type(cell_type_e::REFERENCE), data(data) {}
  cell_c(aberrant_cell_if *acif) : type(cell_type_e::ABERRANT), data(acif) {}
  cell_c(function_info_s fn) : type(cell_type_e::FUNCTION), data(fn) {}

  cell_c() = delete;
  cell_c(const cell_c &other) = delete;
  cell_c(cell_c &&other) = delete;
  cell_c &operator=(const cell_c &other) = delete;
  cell_c &operator=(cell_c &&other) = delete;

  //! \brief Set the cell's canary
  //! \param canary The canary to set
  void set_canary(cell_canary_if *canary) { canary_ = canary; }

  //! \brief From markable_if, check to ensure cell
  //!        is orphaned.
  void verify_marked_status() override {
    if (canary_ == nullptr) {
      this->mark_as_in_use(false);
    }
  }

  //! \brief Destroy the cell
  //! \note If the cell contains a list, the list have each
  //!       of its members marked to be collected
  ~cell_c();

  //! \brief Clone the current cell
  cell_c *clone();

  cell_type_e type{cell_type_e::NIL};
  std::any data{0};
  locator_ptr locator{nullptr};

  //! \brief Update the cell data and type to match another cell
  //! \param other The other cell to match
  //! \note This will not update the locator
  void update_data_and_type_to(cell_c &other);

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
  //! \throws cell_access_exception_c if the cell can not access data as its
  //!         listed type
  std::string to_string();

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
  //! \throws cell_access_exception_c if the cell is not a reference type
  cell_c *to_referenced_cell();

  //! \brief Get a copy of the cell value
  //! \throws cell_access_exception_c if the cell is not an aberrant type
  aberrant_cell_if *as_aberrant();

  //! \brief Get a copy of the cell value
  //! \throws cell_access_exception_c if the cell is not a function type
  function_info_s &as_function_info();

  //! \brief Check if a cell is a numeric type
  bool is_numeric() const {
    return type == cell_type_e::INTEGER || type == cell_type_e::DOUBLE;
  }

private:
  // Canary to ensure the cell is not orphaned. If the canary is null,
  // then the cell is orphaned and should be collected so it will mark
  // itself as not in use.
  // This is just a "double check" to ensure that the cell gets collected
  // if a user overwrites something.
  cell_canary_if *canary_{nullptr};
};
