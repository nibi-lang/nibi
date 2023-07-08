#pragma once

#include "libnibi/cell.hpp"
#include "libnibi/error.hpp"
#include "libnibi/interfaces/instruction_processor_if.hpp"
#include "libnibi/source.hpp"
#include "libnibi/types.hpp"
#include "token.hpp"
#include <functional>
#include <istream>
#include <memory>
#include <optional>
#include <stack>
#include <string_view>
#include <vector>

namespace nibi {

class intake_c {
public:
  intake_c() = delete;

  //! \brief Construct the intake
  //! \param error_cb Callback to handle errors
  //! \param sm Source manager to use for source tracking
  //! \param router Map of symbols to their implementations
  intake_c(instruction_processor_if &processor, error_callback_f error_cb,
           source_manager_c &sm, function_router_t &router);

  //! \brief Read from a stream
  //! \param source Name of the source
  //! \param is Stream to read from
  void read(std::string_view source, std::istream &is);

  //! \brief Read from a string
  //! \param processor Processor to use
  void read_line(std::string_view line,
                 std::shared_ptr<source_origin_c> origin);

  //! \brief Evaluate a line
  //! \param line Line to evaluate
  //! \param origin Source origin
  //! \param location Location of the line source
  void evaluate(std::string_view line, std::shared_ptr<source_origin_c> origin,
                locator_ptr location);

  //! \brief Indicate the end of a file
  void end_of_file();

private:
  /*
      Simple grammar that defines list building

      program           - <instruction_list>+
      instruction_list  - '(' callable <element>+ ')'
      callable          - symbol | access_list
      access_list       - '{' <symbol>+ '}'
      data_list         - '[' <element>+ ']'
      element           - data | list
      list              - instruction_list | data_list | access_list
      data              - symbol | number | string | nil
      number            - integer | real | boolean
      integer           - [-]? <digit>+
      real              - [-]? <integer> '.' <integer> | not-a-number (NaN) |
     inf string            - '"' <any> '"' symbol            - <any>
   */
  class parser_c {
  public:
    parser_c() = delete;
    parser_c(function_router_t &router, error_callback_f ecb)
        : symbol_router_(router), error_cb_(ecb){};
    cell_ptr parse(std::vector<token_c> &tokens);
    bool has_next() { return index_ < tokens_->size(); }
    void next() {
      if (index_ >= tokens_->size()) {
        return;
      }
      index_++;
    }
    token_e current_token() {
      if (index_ >= tokens_->size()) {
        return token_e::NIL;
      }
      return (*tokens_)[index_].get_token();
    }

    locator_ptr current_location() { return (*tokens_)[index_].get_locator(); }
    std::string current_data() { return (*tokens_)[index_].get_data(); }

  private:
    std::size_t index_{0};
    std::vector<token_c> *tokens_{nullptr};
    function_router_t &symbol_router_;
    cell_list_t current_list_;
    error_callback_f error_cb_;

    inline cell_ptr instruction_list();
    inline cell_ptr access_list();
    inline cell_ptr data_list();

    inline cell_ptr list();
    inline cell_ptr data();
    inline cell_ptr element();

    inline cell_ptr symbol();
    inline cell_ptr number();
    inline cell_ptr integer();
    inline cell_ptr boolean();
    inline cell_ptr real();
    inline cell_ptr string();
    inline cell_ptr nil();
  };

  struct tracker_s {
    std::stack<locator_ptr> instruction_stack_;
    std::stack<locator_ptr> data_stack_;
    std::stack<locator_ptr> access_stack_;
    std::size_t line_count{0};
  };

  tracker_s tracker_;
  instruction_processor_if &processor_;
  error_callback_f error_cb_;
  source_manager_c &sm_;
  function_router_t &symbol_router_;
  std::vector<token_c> tokens_;
  std::unique_ptr<parser_c> parser_;

  void check_for_complete_expression();

  bool process_line(std::string_view line,
                    std::shared_ptr<source_origin_c> origin,
                    locator_ptr loc_override = nullptr);

  void process_token(token_c token);
};

} // namespace nibi
