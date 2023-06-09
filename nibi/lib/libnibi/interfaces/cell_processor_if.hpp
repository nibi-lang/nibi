#pragma once

#include "libnibi/cell.hpp"
#include "libnibi/environment.hpp"

namespace nibi {

//! \brief Process a cell and return the value
class cell_processor_if {
public:
  virtual ~cell_processor_if() = default;

  //! \brief Execute a single instruction
  //! \param instruction The instruction to execute
  //! \param env The environment that will be used during execution
  //! \param process_data_cell If true, a data list [] will be iterated and each
  //! item processed
  //! \return The result of executing the instruction
  virtual cell_ptr process_cell(cell_ptr instruction, env_c &env,
                                bool process_data_cell = false) = 0;
};

} // namespace nibi
