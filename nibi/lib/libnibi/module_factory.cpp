#include "module_factory.hpp"
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

module_viewer_ptr module_factory_c::module_viewer() {
  return std::make_shared<module_viewer_c>();
}

} // namespace nibi
