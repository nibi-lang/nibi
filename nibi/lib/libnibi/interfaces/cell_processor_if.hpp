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

  //! \brief Check if the interpreter is yielding a value
  virtual bool is_yielding() = 0;

  //! \brief Get the yield valueq
  virtual cell_ptr get_yield_value() = 0;

  //! \brief Stop recursing on the current instruction and return a value
  virtual void set_yield_value(cell_ptr value) = 0;

  //! \brief Get the last result of the processor
  //! \return The last result
  virtual cell_ptr get_last_result() = 0;

  //! \brief Get the interpreter environment
  //! \return The interpreter environment
  virtual env_c &get_env() = 0;

  //! \brief Get the source manager
  virtual source_manager_c &get_source_manager() = 0;

  //! \brief Load a module
  //! \param module_name The name of the module to load
  virtual void load_module(cell_ptr &module_name) = 0;
};

} // namespace nibi
