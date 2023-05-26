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

class interpreter_factory_c {
public:
  interpreter_factory_c() = default;
  ~interpreter_factory_c() = default;
  interpreter_factory_c(const interpreter_factory_c &) = delete;
  interpreter_factory_c(interpreter_factory_c &&) = delete;
  interpreter_factory_c &operator=(const interpreter_factory_c &) = delete;
  interpreter_factory_c &operator=(interpreter_factory_c &&) = delete;

  //! \brief Create a file interpreter.
  //! \param error_callback Callback for errors.
  //! \return A file interpreter interface shared pointer
  static file_interpreter_ptr file_interpreter(error_callback_f error_callback);

  //! \brief Create a line interpreter.
  //! \param error_callback Callback for errors.
  //! \return A line interpreter interface shared pointer
  static line_interpreter_ptr line_interpreter(error_callback_f error_callback);
};

} // namespace nibi
