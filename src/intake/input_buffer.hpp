#pragma once

#include <cstdint>

#include "token.hpp"
#include "parser.hpp"
#include "source.hpp"

namespace nibi {

//! \brief Handles incrimental input of text until a 
//!        full nibi statement is suspected, then 
//!        hands off the tokens to a parser for evaluation
class input_buffer_c {
public:
  input_buffer_c(parser_c &parser)
    : parser_(parser){}

  //! \brief Submit string data
  //! \note If override_col is set, the value of col will be set 
  //!       to whatever value is given on that particular call.
  void submit(source_origin_ptr &origin, const std::string& data, size_t line_no, bool override_col = false, size_t col = 0);

  //! \brief Indicate that the end of input has been reached
  void end_ind();

private:
  parser_c &parser_;
  size_t line_{0};
  size_t col_{0};
  struct tracker_s {
    uint64_t data{0};
    uint64_t access{0};
    uint64_t ins{0};

    bool all_closed() const { 
      return (
          (data == 0) &&
          (access == 0) &&
          (ins == 0));
    }
  } tracker_;
  std::vector<token_c> tokens_;

  void check_for_eval();

  void tokenize(source_origin_ptr &origin, const std::string& data);
};

} // namespace nibi
