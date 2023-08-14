#pragma once 
#include "token.hpp"
#include <vector>

#include "bytecode/bytecode.hpp"

namespace nibi {

class parser_c {
public:

  parser_c( /* TODO: Make a target to recieve instructions */ );

  void tokens_ind(std::vector<token_c> &tokens);

private:
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
  
  bool debug_{false};
  std::size_t index_{0};
  std::vector<token_c> *tokens_{nullptr};
  std::vector<bytecode::instruction_s> instructions_;

  inline bool instruction_list();
  inline bool access_list();
  inline bool data_list();

  inline bool list();
  inline bool data();
  inline bool element();

  inline bool symbol();
  inline bool number();
  inline bool integer();
  inline bool boolean();
  inline bool real();
  inline bool string();
  inline bool cchar();
};

}// namespace nibi
