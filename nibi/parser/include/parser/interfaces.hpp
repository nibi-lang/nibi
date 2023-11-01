#pragma once

#include "types.hpp"

namespace parser {

//! \brief Basic interface for receiving parsed
//!        lists and associated errors/ etc
class receiver_if {
public:
  //! \brief Called in the event of a parsing error
  virtual void on_error(error_s) = 0;
  //! \brief Called when a full list has been parsed
  virtual void on_list(list_t) = 0;
  //! \brief Called when a top-most list has been completly parsed
  //!        after all potential sub-lists
  virtual void on_top_list_complete() = 0;
};

} // namespace
