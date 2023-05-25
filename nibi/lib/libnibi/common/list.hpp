#pragma once

#include "input.hpp"
#include "token.hpp"

#include "libnibi/cell.hpp"
#include "libnibi/interfaces/instruction_processor_if.hpp"
#include "libnibi/source.hpp"
#include <optional>
#include <vector>

namespace nibi {

//! \brief A list builder that constructs lists as tokens are
//!        scanned and sent to it. Once a full list is detected
//!        it will be sent to the callback to be analyzed
class list_builder_c final : public scanner_cb_if {
public:
  list_builder_c() = delete;
  list_builder_c(instruction_processor_if &cb);
  void on_token(token_c token, bool end_list = false) override;
  void on_error(error_c error) override;
  void on_complete(std::optional<unclosed_type_e> unclosed_symbol) override;

private:
  instruction_processor_if &cb_;
  std::vector<token_c> tokens_;
  std::vector<error_c> errors_;
};
} // namespace nibi
