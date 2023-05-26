#pragma once

#include <filesystem>

namespace nibi {

//! \brief Interface for file interpreters.
class file_interpreter_if {
public:
  virtual ~file_interpreter_if() = default;

  //! \brief Interpret a file.
  virtual void interpret_file(std::filesystem::path file) = 0;

  //! \brief Indicate that interpretation is complete.
  virtual void indicate_complete() = 0;
};

} // namespace nibi
