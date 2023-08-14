#include "input_buffer.hpp"
#include "shared.hpp"
#include "nibi.hpp"
#include <regex>
#include <limits>
#include <iostream>

namespace nibi {

namespace {

std::regex is_number("[+-]?([0-9]*[.])?[0-9]+");

inline bool check_buffer(std::string &buffer, char c) {
  auto it = shared::char_escape_map.find(c);
  if (it != shared::char_escape_map.end()) {
    buffer += it->second;
    return true;
  }
  return false;
}

static std::map<std::string, token_e> type_map = {
    {"nil", token_e::NIL},
    {"true", token_e::TRUE},
    {"false", token_e::FALSE},
    {"nan", token_e::NOT_A_NUMBER},
    {"inf", token_e::INF}};

token_c generate_type_token(token_e token, locator_ptr locator) {
  switch (token) {
  case token_e::NIL:
    return token_c(locator, token_e::NIL, "");
  case token_e::TRUE:
    return token_c(locator, token_e::TRUE, "1");
  case token_e::FALSE:
    return token_c(locator, token_e::FALSE, "0");
  case token_e::NOT_A_NUMBER:
    return token_c(locator, token_e::NOT_A_NUMBER,
                   std::to_string(std::numeric_limits<double>::quiet_NaN()));
  case token_e::INF:
    return token_c(locator, token_e::INF,
                   std::to_string(std::numeric_limits<double>::infinity()));
  }
  std::cerr << "This should never happen > unhandled type for generate_type_token" << std::endl;
  std::exit(1);
}

} // namespace 

void input_buffer_c::end_ind() {

  check_for_eval();

  if (!tracker_.all_closed()) {
    fatal_error(
        error_origin_e::LEXER, 
        nullptr, 
        "End of input indicated to input buffer while parsing instruction");
  }
}

void input_buffer_c::check_for_eval() {
  if (tokens_.size() && tracker_.all_closed()) {
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

#define SUB_NZ(x, msg) \
  if (x == 0) { \
    fatal_error(error_origin_e::LEXER, locator, msg);\
    return; \
  } else { \
    --x;\
  }

void input_buffer_c::tokenize(source_origin_ptr &origin, const std::string &data) {

  for(size_t col = 0; col < data.size(); col++) {

    auto locator = origin->get_locator(line_, col + col_);

    if (std::isspace(data[col])) continue;

    switch (data[col]) {
      case '#': return;
      case '(': {
        tokens_.emplace_back(token_c(locator, token_e::L_PAREN));
        tracker_.ins++;
        break;
      }
      case ')': {
        tokens_.emplace_back(token_c(locator, token_e::R_PAREN));
        SUB_NZ(tracker_.ins, "Unmatched closing paren");
        break;
      }
      case '[': {
        tokens_.emplace_back(token_c(locator, token_e::L_BRACKET));
        tracker_.data++;
        break;
      }
      case ']': {
        tokens_.emplace_back(token_c(locator, token_e::R_BRACKET));
        SUB_NZ(tracker_.data, "Unmatched closing bracket");
        break;
      }
      case '{': {
        tokens_.emplace_back(token_c(locator, token_e::L_BRACE));
        tracker_.access++;
        break;
      }
      case '}': {
        tokens_.emplace_back(token_c(locator, token_e::R_BRACE));
        SUB_NZ(tracker_.access, "Unmatched closing brace");
        break;
      }
    case '\'': {
      bool in_str{true};
      std::string value = "'";
      decltype(col) start = col++;
      while (col < data.size()) {
        if (data[col] == '\'') {
          if (col > 0 && data[col - 1] != '\\') {
            value += data[col];
            break;
          }
        }
        if (data[col] == '\\' && col + 1 < data.size()) {
          if (check_buffer(value, data[col + 1])) {
            col += 2;
            continue;
          }
        }
        value += data[col++];
      }

      if (!value.ends_with('\'')) {
        fatal_error(error_origin_e::LEXER, locator, "Unterminated char");
        return;
      }

      // Remove the quotes
      value = value.substr(1, value.size() - 2);

      tokens_.emplace_back(token_c(locator, token_e::RAW_CHAR, value));
      break;
    }
    case '"': {
      bool in_str{true};
      std::string value = "\"";
      decltype(col) start = col++;
      while (col < data.size()) {
        if (data[col] == '"') {
          if (col > 0 && data[col - 1] != '\\') {
            value += data[col];
            break;
          }
        }
        if (data[col] == '\\' && col + 1 < data.size()) {
          if (check_buffer(value, data[col + 1])) {
            col += 2;
            continue;
          }
        }
        value += data[col++];
      }

      if (!value.ends_with('"')) {
        fatal_error(error_origin_e::LEXER, locator, "Unterminated string");
        return;
      }

      // Remove the quotes
      value = value.substr(1, value.size() - 2);

      tokens_.emplace_back(token_c(locator, token_e::RAW_STRING, value));
      break;
    }

    default: {

      if (std::isdigit(data[col]) || data[col] == '-') {

        // check for negative number vs subtraction
        if (data[col] == '-' && col + 1 < data.size() &&
            !std::isdigit(data[col + 1])) {
          tokens_.emplace_back(token_c(locator, token_e::SYMBOL, "-"));
          break;
        } else if (data[col] == '-' && col == data.size() - 1) {
          tokens_.emplace_back(token_c(locator, token_e::SYMBOL, "-"));
          break;
        }

        std::string number;
        number += data[col];
        while (col + 1 < data.size() &&
               (std::isdigit(data[col + 1]) || data[col + 1] == '.')) {
          number += data[col + 1];
          col++;
        }

        if (std::regex_match(number, is_number)) {
          if (number.find('.') != std::string::npos) {
            tokens_.emplace_back(token_c(locator, token_e::RAW_FLOAT, number));
          } else {
            tokens_.emplace_back(token_c(locator, token_e::RAW_INTEGER, number));
          }
          break;
        } else {
          fatal_error(error_origin_e::LEXER, locator, "Malformed numerical value");
          return;
        }
        break;
      }

      std::string word;
      word += data[col];
      while (col + 1 < data.size() && !std::isspace(data[col + 1]) &&
             data[col + 1] != '(' && data[col + 1] != ')' &&
             data[col + 1] != '[' && data[col + 1] != ']' &&
             data[col + 1] != '{' && data[col + 1] != '}') {
        word += data[col + 1];
        col++;
      }

      {
        auto item = type_map.find(word);
        if (item != type_map.end()) {
          tokens_.emplace_back(generate_type_token(item->second, locator));
          break;
        }
      }

      tokens_.emplace_back(token_c(locator, token_e::SYMBOL, word));
      break;
    }

    } // end switch

  } // end loop

} // end fn

} // namespace nibi
