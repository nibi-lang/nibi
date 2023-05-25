#pragma once

#include "error.hpp"
#include "interfaces/file_interpreter_if.hpp"
#include "interfaces/module_viewer_if.hpp"
#include "source.hpp"
#include "types.hpp"
#include <functional>
#include <memory>

namespace nibi {

class nibi_factory_c {
public:
  nibi_factory_c() = default;
  ~nibi_factory_c() = default;
  nibi_factory_c(const nibi_factory_c &) = delete;
  nibi_factory_c(nibi_factory_c &&) = delete;
  nibi_factory_c &operator=(const nibi_factory_c &) = delete;
  nibi_factory_c &operator=(nibi_factory_c &&) = delete;

  //! \brief Create a module viewer
  module_viewer_ptr module_viewer();

  //! \brief Create a file interpreter.
  //! \param error_callback Callback for errors.
  //! \return A file interpreter interface shared pointer
  file_interpreter_ptr file_interpreter(error_callback_f error_callback);

  // TODO: Add one for repl
  //

  // TODO: Could add one for compiler
};

} // namespace nibi
