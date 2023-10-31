#pragma once

#include "libnibi/RLL/rll_wrapper.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/environment.hpp"
#include "libnibi/error.hpp"
#include "libnibi/interfaces/instruction_processor_if.hpp"
#include "libnibi/modules.hpp"
#include "libnibi/source.hpp"

#include <stack>

#define PROFILE_INTERPRETER 0

namespace nibi {
//! \brief The runtime object that will be used to execute the code
//!        that is generated by the list builder
class interpreter_c final : public instruction_processor_if {
public:
  //! \brief A runtime exception
  class exception_c final : public std::exception {
  public:
    exception_c() = delete;

    //! \brief Construct a new exception
    //! \param message The message that will be printed
    exception_c(std::string message) : message_(message) {}

    //! \brief Construct a new exception
    //! \param message The message that will be printed
    //! \param source_location The location in the source code
    exception_c(std::string message, locator_ptr source_location)
        : message_(message), source_location_(source_location) {}
    char *what() { return const_cast<char *>(message_.c_str()); }
    locator_ptr get_source_location() const { return source_location_; }

  private:
    std::string message_;
    locator_ptr source_location_{nullptr};
  };

  //! \brief Construct a new interpreter object
  //! \param env The object that will used as the top level environment
  //! \param source_manager The source manager that will be used to track
  //!        imported files
  interpreter_c(env_c &env, source_manager_c &source_manager);

  ~interpreter_c();

  // From instruction_processor_if
  void instruction_ind(cell_ptr &cell) override;

  // From interpreter_c
  cell_ptr process_cell(cell_ptr instruction, env_c &env,
                        const bool process_data_cell = false);

  void indicate_repl() { flags_.repl_mode = true; };

  void set_yield_value(cell_ptr value) { stored_cells_.yield_value = value; }

  bool is_yielding() { return stored_cells_.yield_value != nullptr; }

  cell_ptr get_yield_value() { return stored_cells_.yield_value; }

  source_manager_c &get_source_manager() { return source_manager_; }

  cell_ptr get_last_result() { return stored_cells_.last_result; }

  env_c &get_env() { return interpreter_env; }

  void defer_execution(cell_ptr ins) {
    if (ctxs_.empty()) {
      push_ctx();
    }
    ctxs_.top().deferred.push_back(ins);
  }

  void push_ctx() { ctxs_.push(ctx_s{}); }

  void pop_ctx(env_c &env);

  void load_module(cell_ptr &module_name);

  void terminate();

  bool is_terminating() const { return flags_.terminate; }

private:
  struct ctx_s {
    std::vector<cell_ptr> deferred;
  };

  struct flags_s {
    bool repl_mode{false};
    bool terminate{false};
  };

  struct stored_cells_s {
    cell_ptr yield_value{nullptr};
    cell_ptr last_result{nullptr};
  };

  env_c &interpreter_env;

  source_manager_c &source_manager_;

  modules_c modules_;

  std::stack<cell_ptr> call_stack_;

  std::stack<ctx_s> ctxs_;

  flags_s flags_;
  stored_cells_s stored_cells_;

  cell_ptr handle_list_cell(cell_ptr &cell, env_c &env, bool process_data_cell);

  void halt_with_error(error_c error);

#if PROFILE_INTERPRETER
  struct profile_info_s {
    int64_t calls{0};
    int64_t time{0};
  };
  std::unordered_map<std::string, profile_info_s> fn_call_data_;
#endif
};

} // namespace nibi