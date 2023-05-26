#pragma once

#include "libnibi/cell.hpp"
#include "libnibi/error.hpp"
#include "libnibi/interfaces/instruction_processor_if.hpp"
#include "libnibi/parallel_hashmap/phmap.hpp"
#include "libnibi/source.hpp"
#include "libnibi/types.hpp"
#include "token.hpp"
#include <functional>
#include <istream>
#include <optional>
#include <string_view>
#include <vector>
#include "libnibi/source.hpp"

namespace nibi {

class intake_c {
public:
  intake_c() = delete;

  //! \brief Construct the intake
  //! \param error_cb Callback to handle errors
  //! \param sm Source manager to use for source tracking
  //! \param router Map of symbols to their implementations
  intake_c(instruction_processor_if &processor, error_callback_f error_cb,
           source_manager_c &sm, function_router_t router);

  //! \brief Read from a stream
  //! \param source Name of the source
  //! \param is Stream to read from
  void read(std::string_view source, std::istream &is);

  //! \brief Read from a string
  //! \param processor Processor to use
  void read_line(std::string_view line, std::shared_ptr<source_origin_c> origin);

  //! \brief Evaluate a line
  //! \param line Line to evaluate
  //! \param origin Source origin
  //! \param location Location of the line source
  void evaluate(std::string_view line, 
      std::shared_ptr<source_origin_c> origin,
      locator_ptr location);
private:
  struct tracker_s {
    std::size_t bracket_count{0};
    std::size_t paren_count{0};
    std::size_t brace_count{0};
    std::size_t line_count{0};
  };

  tracker_s tracker_;
  instruction_processor_if &processor_;
  error_callback_f error_cb_;
  source_manager_c &sm_;
  function_router_t symbol_router_;
  std::vector<token_c> tokens_;

  bool process_line(std::string_view line,
                    std::shared_ptr<source_origin_c> origin,
                    locator_ptr loc_override = nullptr);

  void process_token(token_c token);
  cell_ptr parse(std::vector<token_c> &tokens, cell_ptr current_list = nullptr);
};

} // namespace nibi
