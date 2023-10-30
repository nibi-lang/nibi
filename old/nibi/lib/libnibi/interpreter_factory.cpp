#include "interpreter_factory.hpp"
#include "environment.hpp"
#include "interpreter/builtins/builtins.hpp"
#include "interpreter/interpreter.hpp"
#include "libnibi/front/file_interpreter.hpp"
#include "libnibi/front/intake.hpp"
#include "modules.hpp"
#include "source.hpp"
#include <filesystem>
#include <fstream>

namespace nibi {

namespace {

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

file_interpreter_ptr
interpreter_factory_c::file_interpreter(error_callback_f error_callback) {
  return std::make_unique<file_interpreter_c>(error_callback);
}

line_interpreter_ptr
interpreter_factory_c::line_interpreter(error_callback_f error_callback) {
  return std::make_unique<line_interpreter_c>(error_callback);
}

} // namespace nibi
