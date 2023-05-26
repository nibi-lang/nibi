#include "nibi_factory.hpp"
#include "environment.hpp"
#include "interpreter/builtins/builtins.hpp"
#include "interpreter/interpreter.hpp"
#include "libnibi/common/intake.hpp"
#include "libnibi/common/file_interpreter.hpp"
#include "modules.hpp"
#include "source.hpp"
#include <filesystem>
#include <fstream>

namespace nibi {

namespace {

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

class line_interpreter_c : public line_interpreter_if {
public:
  line_interpreter_c(error_callback_f error_callback)
      : error_callback_(error_callback),
        interpreter_(environment_, source_manager_),
        intake_(interpreter_, error_callback, source_manager_,
                nibi::builtins::get_builtin_symbols_map()) {
    interpreter_.indicate_repl(); 
    source_origin_ = std::make_shared<source_origin_c>("line_interpreter");
  }
  
  void interpret_line(std::string line) override {
    intake_.read_line(line, source_origin_);
  }

  std::string get_result() override {
    return interpreter_.get_last_result()->to_string();
  }

private:
  std::shared_ptr<source_origin_c> source_origin_;
  error_callback_f error_callback_;
  env_c environment_;
  source_manager_c source_manager_;
  interpreter_c interpreter_;
  intake_c intake_;
};

} // namespace

module_viewer_ptr nibi_factory_c::module_viewer() {
  return std::make_shared<module_viewer_c>();
}

file_interpreter_ptr
nibi_factory_c::file_interpreter(error_callback_f error_callback) {
  return std::make_shared<file_interpreter_c>(error_callback);
}

line_interpreter_ptr
nibi_factory_c::line_interpreter(error_callback_f error_callback) {
  return std::make_shared<line_interpreter_c>(error_callback);
}

} // namespace nibi
