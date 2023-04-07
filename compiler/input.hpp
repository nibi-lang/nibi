#pragma once

#include "token.hpp"
#include "error.hpp"
#include "source.hpp"
#include <string_view>
#include <optional>
#include <unordered_map>

//! \brief Enum to indicate the type of unclosed symbol if one exists
enum class unclosed_type_e {
  PAREN,
  BRACKET,
  BRACE
};

//! \brief A callback interface for the scanner that 
//!        will be called when a token is found or an error occurs.
class scanner_cb_if {
public: 
  //! \brief Called when a token is found.
  virtual void on_token(token_c token) = 0;
  //! \brief Called when an error occurs.
  virtual void on_error(error_c error) = 0;
  //! \brief Called when the input is complete, and indicates if there are unclosed symbols.
  //! \param unclosed_symbol The type of unclosed symbol if one exists.
  virtual void on_complete(std::optional<unclosed_type_e> unclosed_symbol) = 0;
};

//! \brief A scanner object.
class scanner_c {
public:
  scanner_c() = delete;
  scanner_c(scanner_cb_if& cb);

  //! \brief Scan a string.
  bool scan_line(std::shared_ptr<source_origin_c> origin, std::string_view data);

  //! \brief Reset the scanner.
  void reset();

  //! \brief Indicate to the scanner that the input is complete.
  void indicate_complete();

protected:
  scanner_cb_if& cb_;
  struct tracker_s {
    std::size_t bracket_count{0};
    std::size_t paren_count{0};
    std::size_t brace_count{0};
    std::size_t line_count{0};
  };
  tracker_s tracker_;
  std::unordered_map<std::string, token_e> keywords_;
};

//! \brief A file reader object.
//!       This object will read a file and pass the contents to the scanner.
class file_reader_c {
public:
  //! \brief Create a file reader object.
  //! \param cb The scanner callback interface.
  file_reader_c(scanner_cb_if &cb, source_manager_c &sm) : scanner_(cb), sm_(sm) {};

  //! \brief Read a file.
  //! \param path The path to the file.
  std::optional<error_c> read_file(std::string_view path);

private:
  scanner_c scanner_;
  source_manager_c &sm_;
};
