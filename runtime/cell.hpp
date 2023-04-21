#pragma once

#include <any>
#include <cstdint>
#include <exception>
#include <list>
#include <functional>
#include <string>
#include "memory.hpp"
#include "compiler/source.hpp"

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
  FUNCTION
};

// Forward declarations
class env_c;
class cell_c;

//! \brief A list of cells
using cell_list_t = std::list<cell_c*>;

//! \brief A function that takes a list of cells and an environment
using cell_fn_t = std::function<cell_c*(cell_list_t, env_c&)>;

//! \brief A cell specific allocator
using cell_memory_manager_t = memory::controller_c<cell_c>;

//! \brief A cell tyope that can be used to store data
//!        in the runtime from an external dynamic library
//!        
//!       This is a callback interface that the external library

class aberrant_cell_if {
public:
  virtual ~aberrant_cell_if() = default;
};

//! \brief An exception that is thrown when a cell is accessed
//!        in a way that does not correspond to its type
class cell_access_exception_c final : public std::exception {
public:
  //! \brief Create the exception
  //! \param message The message to display
  //! \param source_location The location in the source code
  cell_access_exception_c(std::string message,
                          locator_ptr source_location) : message_(message),
                                                         source_location_(source_location) {}

  //! \brief Get the message
  char* what() {
    return const_cast<char*>(message_.c_str());
  }

  //! \brief Get the source location
  locator_ptr get_source_location() const {
    return source_location_;
  }

private:
  std::string message_;
  locator_ptr source_location_;
};

//! \brief A cell
class cell_c final : public memory::markable_if {
public:
  //! \brief Create a cell with a given type
  cell_c(cell_type_e type) : type(type) {}
  cell_c(int64_t data) : type(cell_type_e::INTEGER), data(data) {}
  cell_c(double data) : type(cell_type_e::DOUBLE), data(data) {}
  cell_c(std::string data) : type(cell_type_e::STRING), data(data) {}
  cell_c(cell_list_t list) : type(cell_type_e::LIST), data(list) {}
  cell_c(cell_c* data) : type(cell_type_e::REFERENCE), data(data) {}
  cell_c(aberrant_cell_if* acif) : type(cell_type_e::ABERRANT), data(acif) {}
  cell_c(cell_fn_t fn)
    : type(cell_type_e::FUNCTION), data(fn) {}

  cell_c() = delete;
  cell_c(const cell_c& other) = delete;
  cell_c(cell_c&& other) = delete;
  cell_c& operator=(const cell_c& other) = delete;
  cell_c& operator=(cell_c&& other) = delete;

  cell_type_e type{cell_type_e::NIL};
  std::any data{0};
  locator_ptr locator{nullptr};

  //! \brief Attempt to access the cell as a list
  //! \throws cell_access_exception_c if the cell is not a list
  cell_list_t& as_list();


  // TODO add functions to access the cell as a string, integer, double, etc
  //        Model them after as_list() and throw cell_access_exception_c
  //        if the cell is not the correct type
};

