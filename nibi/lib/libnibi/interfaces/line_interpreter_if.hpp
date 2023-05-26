#pragma once

#include <string>

namespace nibi {

//! \brief Interface for a repl interpreter.
class line_interpreter_if {
public:
  virtual ~line_interpreter_if() = default;

  //! \brief Interpret a line of code.
  virtual void interpret_line(std::string line) = 0;

  //! \brief Get the result of the last line interpreted.
  virtual std::string get_result() = 0;
};

} // namespace nibi

