#pragma once

#include "libnibi/RLL/rll_wrapper.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/common/error.hpp"
#include "libnibi/common/list.hpp"
#include "libnibi/environment.hpp"
#include "libnibi/modules.hpp"
#include "libnibi/source.hpp"
#include <filesystem>
#include <set>

#define PROFILE_INTERPRETER 0

namespace nibi {
//! \brief The runtime object that will be used to execute the code
//!        that is generated by the list builder
class interpreter_c final : public list_cb_if {
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

  //! \brief Construct a new interpreter object
  //! \param env The object that will used as the top level environment
  //! \param source_manager The source manager that will be used to track
  //!        imported files
  interpreter_c(env_c &env, source_manager_c &source_manager);

  //! \brief Destroy the interpreter object
  ~interpreter_c();

  //! \brief Execute the code that is generated by the list builder
  //! \param list_cell The cell that represents the list
  //! \note This function will be called by the list builder only instructions
  //!       directly read from file will be executed by this function
  //!       All other instructions will be stored and called by these lists
  void on_list(cell_ptr list_cell) override;

  //! \brief Execute a single instruction
  //! \param instruction The instruction to execute
  //! \param env The environment that will be used during execution
  //! \param process_data_cell If true, a data list [] will be iterated and each
  //! item processed \return The result of executing the instruction
  cell_ptr execute_cell(cell_ptr instruction, env_c &env,
                        bool process_data_cell = false);

  //! \brief Halt execution and print an error message
  //! \param error The error that will be printed
  //! \post The program will shutdown
  void halt_with_error(error_c error);

  //! \brief Stop recursing on the current instruction and return a value
  void set_yield_value(cell_ptr value) { yield_value_ = value; }

  //! \brief Check if the interpreter is yielding a value
  bool is_yielding() const { return yield_value_ != nullptr; }

  //! \brief Get the yield valueq
  cell_ptr get_yield_value() const { return yield_value_; }

  source_manager_c &get_source_manager() { return source_manager_; }

  //! \brief Clone the interpreter
  interpreter_c *clone() {
    return new interpreter_c(interpreter_env, source_manager_);
  }

  //! \brief Load a module
  //! \param module_name The name of the module to load
  void load_module(cell_ptr &module_name);

private:
  modules_c modules_;

  // The environment that will be used to store and execute
  // the code that is generated by the list builder
  env_c &interpreter_env;

  // Source manager used to track imported files
  source_manager_c &source_manager_;

  // The yield value
  cell_ptr yield_value_{nullptr};

  // Handle a list cell
  cell_ptr handle_list_cell(cell_ptr &cell, env_c &env, bool process_data_cell);

#if PROFILE_INTERPRETER
  struct profile_info_s {
    int64_t calls{0};
    int64_t time{0};
  };
  std::unordered_map<std::string, profile_info_s> fn_call_data_;
#endif
};

} // namespace nibi