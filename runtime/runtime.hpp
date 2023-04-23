#pragma once

#include "common/error.hpp"
#include "common/list.hpp"
#include "common/source.hpp"
#include "environment.hpp"
#include "runtime/cell.hpp"

//! \brief A function similar to the builtins that
//!        will load a lambda function and execute it
//!        using the global runtime object
//! \param list The list containing the lambda function and args
//! \param env The environment that will be used during execution
extern cell_c *execute_suspected_lambda(cell_list_t &list, env_c &env);

//! \brief The runtime object that will be used to execute the code
//!        that is generated by the list builder
class runtime_c final : public list_cb_if {
public:
  //! \brief A runtime exception
  class exception_c final : public std::exception {
  public:
    //! \brief Construct a new exception
    //! \param message The message that will be printed
    //! \param source_location The location in the source code
    exception_c(std::string message, locator_ptr source_location)
        : message_(message), source_location_(source_location) {}
    char *what() { return const_cast<char *>(message_.c_str()); }
    locator_ptr get_source_location() const { return source_location_; }
  private:
    std::string message_;
    locator_ptr source_location_;
  };

  //! \brief Construct a new runtime object
  //! \param env The object that will used as the top level environment
  runtime_c(env_c &env, source_manager_c &source_manager,
            cell_memory_manager_t &instruction_memory);

  //! \brief Execute the code that is generated by the list builder
  //! \param list_cell The cell that represents the list
  //! \note This function will be called by the list builder only instructions
  //!       directly read from file will be executed by this function
  //!       All other instructions will be stored and called by these lists
  void on_list(cell_c *list_cell) override;

  //! \brief Execute a single instruction
  //! \param instruction The instruction to execute
  //! \param env The environment that will be used during execution
  //! \return The result of executing the instruction
  cell_c *execute_cell(cell_c *instruction, env_c &env);

  //! \brief Halt execution and print an error message
  //! \param error The error that will be printed
  //! \post The program will shutdown
  void halt_with_error(error_c error);

  //! \brief Get the runtime memory object for cell allocations
  cell_memory_manager_t &get_runtime_memory() { return runtime_memory_; };

private:

  // The environment that will be used to store and execute
  // the code that is generated by the list builder
  env_c &global_env_;

  // Source manager used to track imported files
  source_manager_c &source_manager_;

  // The memory used by all builders to store instructions
  // that is managed by this runtime
  cell_memory_manager_t &instruction_memory_;

  // The memory used by the runtime to allocate
  // all cells as a result of executing instructions
  cell_memory_manager_t runtime_memory_;
};

//! \brief The global runtime object that will be used to execute the code
extern runtime_c *global_runtime;

//! \brief Initialize the global runtime object
//! \param env The global environment that will be used
//! \param source_manager The source manager that will be used
//! \param instruction_memory The instruction memory that will be used
//! \return true if the global runtime object was initialized successfully
extern bool global_runtime_init(env_c &env, source_manager_c &source_manager,
                                cell_memory_manager_t &instruction_memory);

//! \brief Destroy the global runtime object
//! \note This function should be called before the program exits
extern void global_runtime_destroy();
