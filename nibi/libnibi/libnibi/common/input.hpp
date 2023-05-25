#pragma once

#include "libnibi/error.hpp"
#include "libnibi/source.hpp"
#include "token.hpp"
#include <filesystem>
#include <optional>
#include <string_view>
#include <unordered_map>

namespace nibi {

//! \brief A callback interface for the scanner that
//!        will be called when a token is found or an error occurs.
class scanner_cb_if {
public:
  //! \brief Enum to indicate the type of unclosed symbol if one exists
  enum class unclosed_type_e { PAREN, BRACKET, BRACE };

  //! \brief Called when a token is found.
  virtual void on_token(token_c token, bool end_list = false) = 0;
  //! \brief Called when an error occurs.
  virtual void on_error(error_c error) = 0;
  //! \brief Called when the input is complete, and indicates if there are
  //! unclosed symbols. \param unclosed_symbol The type of unclosed symbol if
  //! one exists.
  virtual void on_complete(std::optional<unclosed_type_e> unclosed_symbol) = 0;
};

//! \brief A scanner object.
class scanner_c {
public:
  scanner_c() = delete;
  scanner_c(scanner_cb_if &cb);

  //! \brief Scan a string.
  //! \param origin The source origin of the string.
  //! \param data The string to scan.
  //! \param loc_override The location to use to offset token locations.
  //! \note The loc_override is meant for in-place evals
  //! \return True if the scan was successful, false otherwise.
  bool scan_line(std::shared_ptr<source_origin_c> origin, std::string_view data,
                 locator_ptr loc_override = nullptr);

  //! \brief Reset the scanner.
  void reset();

  //! \brief Indicate to the scanner that the input is complete.
  void indicate_complete();

protected:
  scanner_cb_if &cb_;
  struct tracker_s {
    std::size_t bracket_count{0};
    std::size_t paren_count{0};
    std::size_t brace_count{0};
    std::size_t line_count{0};
  };
  tracker_s tracker_;
};

//! \brief A file reader object.
//!       This object will read a file and pass the contents to the scanner.
class file_reader_c {
public:
  //! \brief Create a file reader object.
  //! \param cb The scanner callback interface.
  file_reader_c(scanner_cb_if &cb, source_manager_c &sm)
      : scanner_(cb), sm_(sm){};

  //! \brief Read a file.
  //! \param path The path to the file.
  std::optional<error_c> read_file(std::filesystem::path path);

private:
  scanner_c scanner_;
  source_manager_c &sm_;
};

} // namespace nibi
