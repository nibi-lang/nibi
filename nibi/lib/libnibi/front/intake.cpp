#include "intake.hpp"
#include <regex>

namespace nibi {

static std::regex is_number("[+-]?([0-9]*[.])?[0-9]+");

#define PARSER_ENFORCE_CURRENT_CELL(___message)                                \
  if (!current_list) {                                                         \
    error_cb_(error_c(current_token.get_locator(), ___message));               \
    return nullptr;                                                            \
  }

#define PARSER_ADD_CELL                                                        \
  auto &current_cell_list = current_list->as_list();                           \
  current_cell_list.push_back(cell);                                           \
  return parse(tokens, current_list);

intake_c::intake_c(instruction_processor_if &proc, error_callback_f error_cb,
                   source_manager_c &sm, function_router_t router)
    : processor_(proc), error_cb_(error_cb), sm_(sm), symbol_router_(router) {}

void intake_c::read(std::string_view source, std::istream &is) {

  std::string line;
  auto source_origin = sm_.get_source(std::string(source));

  bool continue_intake = true;
  while (continue_intake && std::getline(is, line)) {
    tracker_.line_count++;
    continue_intake = process_line(line, source_origin);
  }
}

void intake_c::read_line(std::string_view line,
                         std::shared_ptr<source_origin_c> origin) {
  tracker_.line_count++;
  process_line(line, origin);
}

void intake_c::evaluate(std::string_view data,
                        std::shared_ptr<source_origin_c> origin,
                        locator_ptr location) {
  process_line(data, origin, location);
}

bool intake_c::process_line(std::string_view data,
                            std::shared_ptr<source_origin_c> origin,
                            locator_ptr loc_override) {

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
      process_token(token_c(locator, token_e::L_PAREN));
      break;
    }
    case ')': {
      if (tracker_.paren_count == 0) {
        error_cb_(error_c(locator, "Unmatched closing paren"));
        return false;
      }
      tracker_.paren_count--;
      process_token(token_c(locator, token_e::R_PAREN));
      break;
    }
    case '[': {
      tracker_.bracket_count++;
      process_token(token_c(locator, token_e::L_BRACKET));
      break;
    }
    case ']': {
      if (tracker_.bracket_count == 0) {
        error_cb_(error_c(locator, "Unmatched closing bracket"));
        return false;
      }
      tracker_.bracket_count--;
      process_token(token_c(locator, token_e::R_BRACKET));
      break;
    }
    case '{': {
      tracker_.brace_count++;
      process_token(token_c(locator, token_e::L_BRACE));
      break;
    }
    case '}': {
      if (tracker_.brace_count == 0) {
        error_cb_(error_c(locator, "Unmatched closing brace"));
        return false;
      }
      tracker_.brace_count--;
      process_token(token_c(locator, token_e::R_BRACE));
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
        // std::cout << "<<" << value << ">>\n";
        error_cb_(error_c(locator, "Unterminated string"));
        return false;
      }

      // Remove the quotes
      value = value.substr(1, value.size() - 2);

      process_token(token_c(locator, token_e::RAW_STRING, value));
      break;
    }
    default: {

      if (std::isdigit(data[col]) || data[col] == '-') {

        // check for negative number vs subtraction
        if (data[col] == '-' && col + 1 < data.size() &&
            !std::isdigit(data[col + 1])) {
          process_token(token_c(locator, token_e::SYMBOL, "-"));
          break;
        } else if (data[col] == '-' && col == data.size() - 1) {
          process_token(token_c(locator, token_e::SYMBOL, "-"));
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
            process_token(token_c(locator, token_e::RAW_FLOAT, number));
          } else {
            process_token(token_c(locator, token_e::RAW_INTEGER, number));
          }
          break;
        } else {
          error_cb_(error_c(locator, "Malformed numerical value"));
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

      process_token(token_c(locator, token_e::SYMBOL, word));
      break;
    }
    }
  }
  return true;
}

void intake_c::process_token(token_c token) {

  tokens_.push_back(token);

  if (tracker_.paren_count != 0) {
    return;
  }

  auto instruction = parse(tokens_, nullptr);

  if (instruction && instruction->as_list().size()) {
    processor_.instruction_ind(instruction);
  }

  tokens_.clear();
  return;
}

cell_ptr intake_c::parse(std::vector<token_c> &tokens, cell_ptr current_list) {

  if (tokens.empty()) {
    return nullptr;
  }

  auto current_token = tokens[0];

  tokens = std::vector<token_c>(tokens.begin() + 1, tokens.end());

  switch (current_token.get_token()) {
  case token_e::L_BRACE:
    [[fallthrough]];
  case token_e::L_BRACKET: {
    // Data lists must exist within an executable list ([ ... ]])
    PARSER_ENFORCE_CURRENT_CELL("Unexpected opening symbol `{ [`")
    [[fallthrough]];
  }
  case token_e::L_PAREN: {
    auto new_list = allocate_cell(cell_type_e::LIST);
    new_list->locator = current_token.get_locator();

    if (current_token.get_token() == token_e::L_PAREN) {
      new_list->as_list_info().type = list_types_e::INSTRUCTION;
    }
    if (current_token.get_token() == token_e::L_BRACKET) {
      new_list->as_list_info().type = list_types_e::DATA;
    }
    if (current_token.get_token() == token_e::L_BRACE) {
      new_list->as_list_info().type = list_types_e::ACCESS;
    }

    parse(tokens, new_list);

    if (current_list) {
      auto &current_list_actual = current_list->as_list();
      current_list_actual.push_back(new_list);
      return parse(tokens, current_list);
    } else {
      return new_list;
    }
  }
  case token_e::R_BRACKET:
    [[fallthrough]];
  case token_e::R_BRACE:
    [[fallthrough]];
  case token_e::R_PAREN: {
    if (!current_list) {
      error_cb_(error_c(current_token.get_locator(),
                        "Unexpected closing symbol `} ] )`"));
    }
    return nullptr;
  }

  case token_e::SYMBOL: {
    auto symbol_raw = current_token.get_data();
    PARSER_ENFORCE_CURRENT_CELL("Unexpected symbol: " + symbol_raw);

    auto router_location = symbol_router_.find(symbol_raw);

    if (router_location == symbol_router_.end()) {
      auto cell = allocate_cell(symbol_s{symbol_raw});
      cell->locator = current_token.get_locator();
      PARSER_ADD_CELL
    }

    auto cell = allocate_cell(router_location->second);
    cell->locator = current_token.get_locator();

    PARSER_ADD_CELL
  }

  case token_e::RAW_INTEGER: {
    PARSER_ENFORCE_CURRENT_CELL("Unexpected integer");

    auto stringed_value = current_token.get_data();

    int64_t value_actual{0};
    try {
      value_actual = std::stoll(stringed_value);
    } catch (std::exception &e) {
      error_cb_(error_c(current_token.get_locator(),
                        {"Invalid integer value: " + stringed_value}));
      return nullptr;
    }

    auto cell = allocate_cell((int64_t)value_actual);
    cell->locator = current_token.get_locator();

    PARSER_ADD_CELL
  }

  case token_e::RAW_FLOAT: {
    PARSER_ENFORCE_CURRENT_CELL("Unexpected float");

    auto stringed_value = current_token.get_data();

    double value_actual{0.00};
    try {
      value_actual = std::stod(stringed_value);
    } catch (std::exception &e) {
      error_cb_(error_c(current_token.get_locator(),
                        {"Invalid double value: " + stringed_value}));
      return nullptr;
    }

    auto cell = allocate_cell((double)value_actual);
    cell->locator = current_token.get_locator();

    PARSER_ADD_CELL
  }

  case token_e::RAW_STRING: {
    PARSER_ENFORCE_CURRENT_CELL("Unexpected string");
    auto cell = allocate_cell(current_token.get_data());
    cell->locator = current_token.get_locator();

    PARSER_ADD_CELL
  }
  }
  return nullptr;
}

} // namespace nibi
