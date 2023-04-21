#include "list.hpp"

#include <iostream>
#include <functional>

#include "compiler/error.hpp"

#include "runtime/builtins/builtins.hpp"


#define PARSER_ENFORCE_CURRENT_CELL(___message) \
  if (!current_list) { \
    on_error_(error_c(current_token.get_locator(), ___message)); \
    return nullptr; \
  }

#define PARSER_ADD_CELL \
  auto& current_cell_list = current_list->as_list(); \
  current_cell_list.push_back(cell); \
  return parse(tokens, current_list);

namespace {

class parser_c {
public:
  parser_c() = delete;
  parser_c(cell_memory_manager_t& ins_memory,
           std::function<void(error_c error)> on_error)
    : ins_memory_(ins_memory),
      on_error_(on_error) {}
  cell_c* parse(std::vector<token_c>& tokens, cell_c *current_cell = nullptr);
private:
  cell_memory_manager_t& ins_memory_;
  std::function<void(error_c error)> on_error_;
};

}


list_builder_c::list_builder_c(cell_memory_manager_t& ins_mem, list_cb_if& cb) : ins_memory_(ins_mem), cb_(cb) {

}

void list_builder_c::on_error(error_c error) {
  // TODO: add error to list and allow up-to a certain number of errors
  // before aborting
  std::cout << "Error reported to list builder!\n";
  error.draw_error();
}

void list_builder_c::on_complete(std::optional<unclosed_type_e> unclosed_symbol) {
  // Check for unclosed symbolsq
  // if a symbol is unclosed, then the program can not continue
  // and we need to attempt to find the unclosed symbol and 
  // draw an error for it
}

void list_builder_c::on_token(token_c token, bool end_list) {

  std::cout << "builder received token: " << token_to_string(token) << std::endl;

  // Accumulate tokens until we have a "end_list" condition (all parens closed)
  tokens_.push_back(token);

  if (end_list) {

    // We have a full list, so we need to parse it
    auto new_list = parser_c(
      ins_memory_,
      std::bind(&list_builder_c::on_error, this, std::placeholders::_1)).parse(tokens_);

    // if the list is not empty, then we need to send it to the callback
    // an empty lists means an error was found, and the error was already
    // reported to the callback
    if (new_list) {
      cb_.on_list(new_list);
    }

    tokens_.clear();
    return;
  }
}

cell_c* parser_c::parse(std::vector<token_c> &tokens, cell_c *current_list) {

  if (tokens.empty()) {
    return nullptr;
  }

  auto current_token = tokens[0];

  tokens = std::vector<token_c>(tokens.begin() + 1, tokens.end());


  switch (current_token.get_token()) {
    case token_e::L_BRACKET: {
      // Data lists must exist within an executable list ([ ... ]])
      PARSER_ENFORCE_CURRENT_CELL("Unexpected opening bracket")
      [[fallthrough]];
    }
    case token_e::L_PAREN: {
      auto* new_list = ins_memory_.allocate(cell_type_e::LIST);
      new_list->locator = current_token.get_locator();

      if (current_token.get_token() == token_e::L_PAREN) {
        new_list->as_list_info().type = list_types_e::INSTRUCTION;
      }
      if (current_token.get_token() == token_e::L_BRACKET) {
        new_list->as_list_info().type = list_types_e::DATA;
      }

      parse(tokens, new_list);

      if (current_list) {
        auto& current_list_actual = current_list->as_list();
        current_list_actual.push_back(new_list);
        return parse(tokens, current_list);
      } else {
        return new_list;
      }
    }
    case token_e::R_BRACKET:
      [[fallthrough]];
    case token_e::R_PAREN: {
      if (!current_list) {
        on_error_(
          error_c(current_token.get_locator(), "Unexpected closing parenthesis")
        );
      }
      return nullptr;
    }

    case token_e::ADD: {
      // Check if we have a current cell to add to
      // if we don't there is a syntax error
      PARSER_ENFORCE_CURRENT_CELL("Unexpected add instruction");   

      // Allocate the cell in instruction memory containing the 
      // add instruction
      auto* cell = ins_memory_.allocate(
        function_info_s{
          "+", 
          builtins::builtin_fn_arithmetic_add,
          function_type_e::BUILTIN_CPP_FUNCTION}
      );

      cell->locator = current_token.get_locator();

      PARSER_ADD_CELL
    }

    case token_e::RAW_INTEGER: {
      PARSER_ENFORCE_CURRENT_CELL("Unexpected integer");   

      auto stringed_value = current_token.get_data();

      int64_t value_actual{0};
      try {
        value_actual = std::stoll(stringed_value);
      } catch (std::exception& e) {
        on_error_(
          error_c(current_token.get_locator(), {"Invalid integer value: " + stringed_value})
        );
        return nullptr;
      }

      auto* cell = ins_memory_.allocate(value_actual);
      cell->locator = current_token.get_locator();

      PARSER_ADD_CELL
    }




  }

  return nullptr;
}

