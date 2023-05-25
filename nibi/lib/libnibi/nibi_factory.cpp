#include "nibi_factory.hpp"
#include "environment.hpp"
#include "interpreter/builtins/builtins.hpp"
#include "interpreter/interpreter.hpp"
#include "libnibi/common/intake.hpp"
#include "modules.hpp"
#include "source.hpp"
#include <filesystem>
#include <fstream>

namespace nibi {

namespace {

class file_interpreter_c : public file_interpreter_if {
public:
  file_interpreter_c(error_callback_f error_callback)
      : error_callback_(error_callback),
        interpreter_(environment_, source_manager_),
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

class module_viewer_c : public module_viewer_if {
public:
  module_viewer_c()
      : interpreter_(environment_, source_manager_),
        modules_(source_manager_, interpreter_) {}
  virtual ~module_viewer_c() = default;
  virtual module_info_s get_module_info(std::string &name) override {
    return modules_.get_module_info(name);
  }

private:
  env_c environment_;
  source_manager_c source_manager_;
  interpreter_c interpreter_;
  modules_c modules_;
};

} // namespace

module_viewer_ptr nibi_factory_c::module_viewer() {
  return std::make_shared<module_viewer_c>();
}

file_interpreter_ptr
nibi_factory_c::file_interpreter(error_callback_f error_callback) {
  return std::make_shared<file_interpreter_c>(error_callback);
}

} // namespace nibi
