#include "input.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>

namespace nibi {

static std::regex is_number("[+-]?([0-9]*[.])?[0-9]+");

std::optional<error_c>
file_reader_c::read_file(std::filesystem::path file_path) {
  // Read the file and pass the contents to the scanner.

  if (!std::filesystem::exists(file_path)) {
    return error_c("File does not exist");
  }

  scanner_.reset();

  std::ifstream ifs;
  ifs.open(file_path);
  if (ifs.fail()) {
    return error_c("Failed to open file");
  }

  auto source_origin = sm_.get_source(file_path.string());

  std::string data;
  while (!ifs.eof()) {
    std::getline(ifs, data);
    scanner_.scan_line(source_origin, data);
  }

  ifs.close();

  scanner_.indicate_complete();

  return {};
}

scanner_c::scanner_c(scanner_cb_if &cb) : cb_(cb) {}

void scanner_c::reset() { tracker_ = tracker_s{}; }

void scanner_c::indicate_complete() {
  if (tracker_.paren_count != 0) {
    cb_.on_complete({scanner_cb_if::unclosed_type_e::PAREN});
    return;
  }
  if (tracker_.bracket_count != 0) {
    cb_.on_complete({scanner_cb_if::unclosed_type_e::BRACKET});
    return;
  }
  cb_.on_complete({});
}

bool scanner_c::scan_line(std::shared_ptr<source_origin_c> origin,
                          std::string_view data, locator_ptr loc_override) {

  tracker_.line_count++;
  for (std::size_t col = 0; col < data.size(); col++) {
    auto locator =
        (loc_override)
            ? origin->get_locator(tracker_.line_count,
                                  loc_override->get_column() + 1 + col)
            : origin->get_locator(tracker_.line_count, col);
    if (std::isspace(data[col])) {
      continue;
    }
    switch (data[col]) {
    case '#': {
      return true;
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
      cb_.on_token(token_c(locator, token_e::R_PAREN),
                   tracker_.paren_count == 0);
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
        value += data[col++];
      }

      if (!value.ends_with('"')) {
        cb_.on_error(error_c(locator, "Unterminated string"));
        return false;
      }

      // Remove the quotes
      value = value.substr(1, value.size() - 2);

      cb_.on_token(token_c(locator, token_e::RAW_STRING, value));
      break;
    }
    default: {

      if (std::isdigit(data[col]) || data[col] == '-') {

        // check for negative number vs subtraction
        if (data[col] == '-' && col + 1 < data.size() &&
            !std::isdigit(data[col + 1])) {
          cb_.on_token(token_c(locator, token_e::SYMBOL, "-"));
          break;
        } else if (data[col] == '-' && col == data.size() - 1) {
          cb_.on_token(token_c(locator, token_e::SYMBOL, "-"));
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

      std::string word;
      word += data[col];
      while (col + 1 < data.size() && !std::isspace(data[col + 1]) &&
             data[col + 1] != '(' && data[col + 1] != ')' &&
             data[col + 1] != '[' && data[col + 1] != ']' &&
             data[col + 1] != '}' && data[col + 1] != '}') {
        word += data[col + 1];
        col++;
      }

      cb_.on_token(token_c(locator, token_e::SYMBOL, word));
      break;
    }
    }
  }
  return true;
}

} // namespace nibi
