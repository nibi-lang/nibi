#include "list.hpp"

#include <iostream>
#include <functional>

namespace {

class parser_c {
public:
  parser_c() = delete;
  parser_c(std::vector<token_c> &tokens,
           std::function<void(error_c error)> on_error)
    : tokens_(tokens),
      on_error_(on_error) {}
  std::unique_ptr<list_c> parse();
private:
  std::vector<token_c> &tokens_;
  std::function<void(error_c error)> on_error_;
};

}


list_builder_c::list_builder_c(list_cb_if& cb) : cb_(cb) {

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
      tokens_, 
      std::bind(&list_builder_c::on_error, this, std::placeholders::_1)).parse();

    // if the list is not empty, then we need to send it to the callback
    // an empty lists means an error was found, and the error was already
    // reported to the callback
    if (new_list) {
      cb_.on_list(std::move(new_list));
    }

    tokens_.clear();
    return;
  }
}

std::unique_ptr<list_c> parser_c::parse() {

  std::cout << "Parser received " << tokens_.size() << " tokens\n";

  return nullptr;
}

