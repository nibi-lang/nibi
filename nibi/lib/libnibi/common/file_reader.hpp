#pragma once

#include <filesystem>
#include <fstream>
#include "libnibi/common/intake.hpp"
#include "libnibi/environment.hpp"
#include "libnibi/source.hpp"
#include "libnibi/interpreter/builtins/builtins.hpp"
#include "libnibi/interpreter/interpreter.hpp"
#include "libnibi/interfaces/file_interpreter_if.hpp"

namespace nibi {

class file_interpreter_c : public file_interpreter_if {
public:
  //! \brief Interpret a file.
  //! \param error_callback Callback to report errors.
  file_interpreter_c(error_callback_f error_callback)
      : error_callback_(error_callback),
        interpreter_(environment_, source_manager_),
        intake_(interpreter_, error_callback, source_manager_,
                nibi::builtins::get_builtin_symbols_map()) {}

  //! \brief Interpret a file and populate a specific environment.
  //! \param error_callback Callback to report errors.
  //! \param env Environment to populate.
  file_interpreter_c(
      error_callback_f error_callback,
      env_c &env) : error_callback_(error_callback),
                    interpreter_(env, source_manager_),
                    intake_(interpreter_, error_callback, source_manager_,
                            nibi::builtins::get_builtin_symbols_map()) {}

  //! \brief Interpret a file and populate a specific environment.
  //!        and specific source manager.
  //! \param error_callback Callback to report errors.
  //! \param env Environment to populate.
  //! \param sm Source manager to use.
  file_interpreter_c(
      error_callback_f error_callback,
      env_c &env,
      source_manager_c &sm)
      : error_callback_(error_callback),
                    interpreter_(env, sm),
                    intake_(interpreter_, error_callback, source_manager_,
                            nibi::builtins::get_builtin_symbols_map()) {}
  ~file_interpreter_c() { indicate_complete(); }

  void interpret_file(std::filesystem::path filename) override {
    if (!std::filesystem::exists(filename)) {
      error_callback_(error_c("File does not exist: " + filename.string()));
      return;
    }

    file_.open(filename);
    if (!file_.is_open()) {
      error_callback_(error_c("Could not open file: " + filename.string()));
      return;
    }

    intake_.read(filename.string(), file_);
  }

  void indicate_complete() override {
    if (file_.is_open()) {
      file_.close();
    }
  }

private:
  error_callback_f error_callback_;
  env_c environment_;
  source_manager_c source_manager_;
  interpreter_c interpreter_;
  intake_c intake_;
  std::ifstream file_;
};



}
