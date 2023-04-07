#include "input.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>

static std::regex is_number("[+-]?([0-9]*[.])?[0-9]+");

std::optional<error_c> file_reader_c::read_file(std::string_view path) {
  // Read the file and pass the contents to the scanner.
  std::filesystem::path file_path(path);

  if (!std::filesystem::exists(file_path)) {
    return error_c("File does not exist");
  }

  scanner_.reset();

  std::ifstream ifs; 
  ifs.open(file_path, std::ios::in);
  if (ifs.fail()) {
    return error_c("Failed to open file");
  }

  auto source_origin = sm_.get_source(file_path.string());

  std::string data;
  while(!ifs.eof()) {
    std::getline(ifs, data);
    scanner_.scan_line(source_origin, data);
  }

  ifs.close();

  scanner_.indicate_complete();

  return {};
}

scanner_c::scanner_c(scanner_cb_if& cb) : cb_(cb)
{
  keywords_ = {
    {"...",token_e::VARIADIC},
    {"fn",token_e::FN},
    {"loop",token_e::LOOP},
    {"if",token_e::IF},
    {"len",token_e::LEN},
    {"at",token_e::AT},
    {"view",token_e::VIEW},
    {"quote",token_e::QUOTE},
    {"eval",token_e::EVAL},
    {"drop",token_e::DROP},
    {"type",token_e::TYPE},
    {"quit",token_e::QUIT},
    {"rev",token_e::REV},
    {"sub",token_e::SUB},
    {"await",token_e::AWAIT},
    {"use",token_e::USE},
    {"load",token_e::LOAD},
    {"nil?",token_e::IS_NIL},
    {"promise?",token_e::IS_PROMISE},
    {"exists?",token_e::EXISTS},
    {"$args",token_e::ARGS_LIST},
    {"param",token_e::PARAM_BIND},
    {"mod",token_e::MODULE},
    {"pub",token_e::PUBLIC}
  };
}

void scanner_c::reset() {
  tracker_ = tracker_s{};
}

void scanner_c::indicate_complete() {
  if (tracker_.paren_count != 0) {
    cb_.on_complete({unclosed_type_e::PAREN});
    return;
  }
  if (tracker_.bracket_count != 0) {
    cb_.on_complete({unclosed_type_e::BRACKET});
    return;
  }
  if (tracker_.brace_count != 0) {
    cb_.on_complete({unclosed_type_e::BRACE});
    return;
  }
  cb_.on_complete({});
}

bool scanner_c::scan_line(std::shared_ptr<source_origin_c> origin, std::string_view data) {
  tracker_.line_count++;
  for(std::size_t col = 0; col < data.size(); col++) {
    auto locator = origin->get_locator(tracker_.line_count, col);
    switch(data[col]) {
      case ' ': {
        // ignore whitespace
        break;
      }
      case '(': { 
        tracker_.paren_count++;
        cb_.on_token(token_c(locator, token_e::L_PAREN));
        break;
      }
      case ')': { 
        if (tracker_.paren_count == 0) {
          cb_.on_error(error_c(locator, "Unmatched closing paren"));
          return false;
        }
        tracker_.paren_count--;
        cb_.on_token(token_c(locator, token_e::R_PAREN));
        break;
      }
      case '[': { 
        tracker_.bracket_count++;
        cb_.on_token(token_c(locator, token_e::L_BRACKET));
        break;
      }
      case ']': { 
        if (tracker_.bracket_count == 0) {
          cb_.on_error(error_c(locator, "Unmatched closing bracket"));
          return false;
        }
        tracker_.bracket_count--;
        cb_.on_token(token_c(locator, token_e::R_BRACKET));
        break;
      }
      case '{': { 
        tracker_.brace_count++;
        cb_.on_token(token_c(locator, token_e::L_BRACE));
        break;
      }
      case '}': { 
        if (tracker_.brace_count == 0) {
          cb_.on_error(error_c(locator, "Unmatched closing brace"));
          return false;
        }
        tracker_.brace_count--;
        cb_.on_token(token_c(locator, token_e::R_BRACE));
        break;
      }
      case '<': {
        if (col + 1 < data.size()) {
          auto next = data[col + 1];
          if (next == '=') {
            cb_.on_token(token_c(locator, token_e::LTE));
            col++;
            break;
          } else if (next == '<') {
            cb_.on_token(token_c(locator, token_e::L_SHIFT));
            col++;
            break;
          } else if (next == '-') {
            cb_.on_token(token_c(locator, token_e::RETURN));
            col++;
            break;
          }
        } 
        cb_.on_token(token_c(locator, token_e::LT));
        break;
      }
      case '>': {
        if (col + 1 < data.size()) {
          auto next = data[col + 1];
          if (next == '=') {
            cb_.on_token(token_c(locator, token_e::GTE));
            col++;
            break;
          } else if (next == '>') {
            cb_.on_token(token_c(locator, token_e::R_SHIFT));
            col++;
            break;
          }
        }
        cb_.on_token(token_c(locator, token_e::GT));
        break;
      }
      case ':': {
        if (col + 1 < data.size()) {
          auto next = data[col + 1];
          if (next == '=') {
            cb_.on_token(token_c(locator, token_e::ASSIGNMENT));
            col++;
          } else if (next == ':') {
            cb_.on_token(token_c(locator, token_e::SCOPE));
            col++;
          }
        }
        break;
      }
      case '+': {
        cb_.on_token(token_c(locator, token_e::ADD));
        break;
      }
      case '/': {
        cb_.on_token(token_c(locator, token_e::DIVIDE));
        break;
      }
      case '*': {
        if (col + 1 < data.size()) {
          auto next = data[col + 1];
          if (next == '*') {
            cb_.on_token(token_c(locator, token_e::POWER));
            col++;
            break;
          }
        }
        cb_.on_token(token_c(locator, token_e::MULTIPLY));
        break;
      }
      case '%': {
        cb_.on_token(token_c(locator, token_e::MODULO));
        break;
      }
      case '&': {
        if (col + 1 < data.size()) {
          auto next = data[col + 1];
          if (next == '&') {
            cb_.on_token(token_c(locator, token_e::BITWISE_AND));
            col++;
            break;
          }
        }
        cb_.on_token(token_c(locator, token_e::LOGICAL_AND));
        break;
      }
      case '|': {
        if (col + 1 < data.size()) {
          auto next = data[col + 1];
          if (next == '|') {
            cb_.on_token(token_c(locator, token_e::LOGICAL_OR));
            col++;
            break;
          }
        }
        cb_.on_token(token_c(locator, token_e::BITWISE_OR));
        break;
      }
      case '!': {
        cb_.on_token(token_c(locator, token_e::LOGICAL_NOT));
        break;
      }
      case '~': {
        cb_.on_token(token_c(locator, token_e::BITWISE_NOT));
        break;
      }
      case '^': {
        cb_.on_token(token_c(locator, token_e::BITWISE_XOR));
        break;
      }
      case '"' : {
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
          value += data[col++];
        }

        if (!value.ends_with('"')) {
          std::cout << "<<" << value << ">>\n";
          cb_.on_error(error_c(locator, "Unterminated string"));
          return false;
        }

        cb_.on_token(token_c(locator, token_e::RAW_STRING, value));
        break;
      }
      default: {

        if (std::isdigit(data[col]) || data[col] == '-') {

          // check for negative number vs subtraction
          if (data[col] == '-' && col + 1 < data.size() && !std::isdigit(data[col + 1])) {
            cb_.on_token(token_c(locator, token_e::SUBTRACT));
            break;
          } else if (data[col] == '-' && col == data.size() - 1) {
            cb_.on_token(token_c(locator, token_e::SUBTRACT));
            break;
          }

          std::string number;
          number += data[col];
          while (col + 1 < data.size() && (std::isdigit(data[col + 1]) || data[col + 1] == '.') ) {
            number += data[col + 1];
            col++;
          }

          if (std::regex_match(number, is_number)) {
            if (number.find('.') != std::string::npos) {
              cb_.on_token(token_c(locator, token_e::RAW_FLOAT, number));
            } else {
              cb_.on_token(token_c(locator, token_e::RAW_INTEGER, number));
            }
            break;
          } else {
            cb_.on_error(error_c(locator, "Malformed numerical value"));
            return false;
          }
          break;
        }

        if (std::isalpha(data[col])) {
          std::string word;
          word += data[col];
          while (col + 1 < data.size() && std::isalnum(data[col + 1])) {
            word += data[col + 1];
            col++;
          }

          // check for keywords vs identifiers
          if (keywords_.find(word) != keywords_.end()) {
            cb_.on_token(token_c(locator, keywords_[word]));
          } else {
            cb_.on_token(token_c(locator, token_e::IDENTIFIER, word));
          }
          break;
        }

        cb_.on_error(error_c(locator, "Unexpected character"));
        return false;
      }
    }
  }
  return true;
}