#pragma once

#include <any>
#include <cstdint>
#include <forward_list>
#include <functional>

//! \brief The type of a cell
//! \note The aberrant type is Mysterious externally defined type
//!       meant to be used by external libraries that
//!       need to store data in the cell
enum class cell_type_e {
  NIL,
  INTEGER,
  FLOAT,
  STRING,
  CPP_FUNCTION,
  FUNCTION,
  LIST,
  ABERRANT
};

// Forward declarations
class env_c;
class cell_c;

//! \brief A list of cells
using cell_list_t = std::forward_list<cell_c*>;

//! \brief A function that takes a list of cells and an environment
using cell_fn_t = std::function<cell_c(cell_list_t, env_c&)>;

//! \brief A cell
class cell_c {
public:
  //! \brief Create a cell with a given type
  cell_c(cell_type_e type) : type(type) {}

  cell_c() = delete;
  cell_c(const cell_c& other) = delete;
  cell_c(cell_c&& other) = delete;
  cell_c& operator=(const cell_c& other) = delete;
  cell_c& operator=(cell_c&& other) = delete;

  cell_type_e type{cell_type_e::NIL};
  std::any data{0};
};
