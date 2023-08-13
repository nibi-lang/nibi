#include "input_buffer.hpp"

namespace nibi {

void end_ind() {
 // If we end on an incomplete expr then we need to 
 // call the error thing in nibi.hpp
}

void input_buffer_c::check_for_eval() {
  if (tokens_.size() && tracker_ == 0) {
    parser_.tokens_ind(tokens_);
    tokens_.clear();
  }
}

void input_buffer_c::submit(
    source_origin_ptr &origin,
    const std::string &data,
    size_t line_no,
    bool override_col,
    size_t col) {

  if (line_no != line_) {
    line_ = line_no;
    col_ = 0;
  }

  if (override_col) { col_ = col; }

  tokenize(origin, data);

  return check_for_eval();
}

void input_buffer_c::tokenize(source_origin_ptr &origin, const std::string &data) {


}


} // namespace nibi
