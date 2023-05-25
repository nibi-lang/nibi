#pragma once

#include "error.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/common/instruction_processor_if.hpp"
#include "libnibi/parallel_hashmap/phmap.hpp"
#include "libnibi/source.hpp"
#include "token.hpp"
#include <functional>
#include <istream>
#include <optional>
#include <string_view>
#include <vector>

namespace nibi {

class intake_c {
public:
  //! \brief Error callback that will handle whatever error(s) the intake
  //!        encounters.
  using error_cb_t = std::function<void(nibi::error_c)>;

  //! \brief Map that ties builtin symbols to the functions that implement them.
  //! \note The idea is to pass builtins for interpretation,
  //!       or to pass compilation methods for compilation.
  using function_router_t =
      phmap::parallel_node_hash_map<std::string, function_info_s> &;

  intake_c() = delete;

  //! \brief Construct the intake
  //! \param error_cb Callback to handle errors
  //! \param sm Source manager to use for source tracking
  //! \param router Map of symbols to their implementations
  intake_c(instruction_processor_if &processor, error_cb_t error_cb,
           source_manager_c &sm, function_router_t router);

  //! \brief Read from a stream
  //! \param source Name of the source
  //! \param is Stream to read from
  void read(std::string_view source, std::istream &is);

private:
  struct tracker_s {
    std::size_t bracket_count{0};
    std::size_t paren_count{0};
    std::size_t brace_count{0};
    std::size_t line_count{0};
  };

  tracker_s tracker_;
  instruction_processor_if &processor_;
  error_cb_t error_cb_;
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
