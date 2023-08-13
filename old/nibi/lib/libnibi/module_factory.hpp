#pragma once

#include "error.hpp"
#include "interfaces/file_interpreter_if.hpp"
#include "interfaces/line_interpreter_if.hpp"
#include "interfaces/module_viewer_if.hpp"
#include "source.hpp"
#include "types.hpp"
#include <functional>
#include <memory>

namespace nibi {

class module_factory_c {
public:
  module_factory_c() = default;
  ~module_factory_c() = default;
  module_factory_c(const module_factory_c &) = delete;
  module_factory_c(module_factory_c &&) = delete;
  module_factory_c &operator=(const module_factory_c &) = delete;
  module_factory_c &operator=(module_factory_c &&) = delete;

  //! \brief Create a module viewer
  static module_viewer_ptr module_viewer();
};

} // namespace nibi
