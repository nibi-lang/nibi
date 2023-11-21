#include "atomiser.hpp"

#include <fmt/format.h>
#include <algorithm>
#include <cctype>
#include <map>
#include <regex>

namespace {

static std::regex is_number(R"([+-]?([0-9]*[.])?[0-9]+)");
static std::map<char, char> char_escape_map = {
    {'n', '\n'},  {'t', '\t'},  {'r', '\r'}, {'a', '\a'},
    {'b', '\b'},  {'v', '\v'},  {'?', '\?'}, {'"', '\"'},
    {'\'', '\''}, {'\\', '\\'}, {'0', '\0'}};

inline bool check_for_chars(std::string &buffer, char c) {
  auto it = char_escape_map.find(c);
  if (it != char_escape_map.end()) {
    buffer += it->second;
    return true;
  }
  return false;
}

inline void line_splitter(std::string& data, std::vector<std::string> &lines){
  static std::regex pattern(R"(\n)");
  std::copy(
      std::sregex_token_iterator(data.begin(), data.end(), pattern, -1),
      std::sregex_token_iterator(),
      std::back_inserter(lines));
}

inline std::string& rtrim(std::string &line) {
    line.erase(std::find_if(line.rbegin(), line.rend(), 
    [](int ch) {
        return !std::isspace(ch);
    }).base(), line.end());
    return line;
}

inline std::string& ltrim (std::string &line) {
    line.erase(line.begin(),find_if_not(line.begin(),line.end(),
    [](int c) {
        return std::isspace(c);
    }));
    return line;
}

inline std::string& trim(std::string& line) {
  return ltrim(rtrim(line));
}

inline std::string normalize(std::string &data) {
  std::vector<std::string> lines;
  line_splitter(data, lines);
  std::string result;
  std::for_each(
    lines.begin(),
    lines.end(),
    [&](std::string &line) {
      if (trim(line).empty()) return line;
      if (line[0] == '#') return line;
      line = line.substr(0, line.find(";"));
      if (line.size()) result = fmt::format("{} {}", result, line);
      return line;
    });
  //for(auto &&line : lines) {
  //  fmt::print("line: {}\n", line);
  //}
  return result;
}

} // namespace

atomiser_c::atomiser_c(atom_receiver_if &receiver)
    : _receiver(receiver) {
  _termChars = {'{', '}', '[', ']', '(', ')'};
}

void atomiser_c::add_to_buffer(const std::string& data) {
  if (data.empty()) return;
  std::size_t line = _trace.line;
  std::size_t col = _trace.col;
  for(const auto& c : data) {
    if ('(' == c) { _buffer.tracker++; }
    else if (')' == c) {
      if (_buffer.tracker == 0) {
        _receiver.on_error(
          line, col, "Mismatched parentheses");
        return reset();
      }
      _buffer.tracker--;
    }
    _buffer.data += c;
    col++;
  }
}

void atomiser_c::submit(const char* data, size_t line) {
  if (!data) {
    return;
  }
  std::string val = data;
  return submit(val, line);
}

void atomiser_c::submit(std::string &data, size_t line) {

  /*
        TODO:

        Right now we can submit any number of lines to the 
        buffer and it "normalizes" it into one long string.
        This is great for processing, but we may wan't to preserve
        line and column information to ensure that multi-lined
        input can be tracked correctly, as the tracker.line/col
        is the line/col of the buffer, not the input.

        This isn't an issue iff all input is one statement-per-line
        and it is fed to us one line at a time, but we may 
        want to just dump an entire file in....


    */
  add_to_buffer(normalize(data));

  if (!_buffer.ready()) {
    return;
  }

  _trace.line = line;
  _trace.col = 0;

  while(!at_end()) {
    _receiver.on_list(parse(nullptr));

/*
    // Show the buffer, and arrow in on where the last 
    // processing had stopped

    fmt::print("{}\n", _buffer.data);
    for(auto i = 0; i < _trace.col-1; i++) {
      fmt::print(" ");
    }
    fmt::print("^\n");
*/
  }

  reset();
}

void atomiser_c::indicate_complete() {
  if (_buffer.data.empty()) {
    return;
  }

  emit_error("Atomiser containes buffered data");
}

atom_list_t atomiser_c::parse(atom_list_t *list) {

  while(_trace.col < _buffer.data.size()) {

    auto c = _buffer.data[_trace.col];

    if (std::isspace(c)) {
      _trace.col++;
      continue;
    }

    switch (c) {
      case '(': {
        std::size_t line = _trace.line;
        std::size_t col = _trace.col;

        _trace.col++;

        atom_list_t new_list;
        auto result = parse(&new_list);

        if (list) {
          list->push_back(
            std::make_unique<atom_list_c>(
              std::move(parse(&result)), file_position_s{line, col}));
          return std::move(*list);
        }

        return std::move(result);
      }

      case ')':
        _trace.col++;
        return std::move(*list);

      case '"': {
        bool in_str{true};
        std::string value = "\"";
        decltype(_trace.col) start = _trace.col++;
        while (_trace.col < _buffer.data.size()) {
         if (_buffer.data[_trace.col] == '"') {
            if (_trace.col > 0 && _buffer.data[_trace.col - 1] != '\\') {
              value += _buffer.data[_trace.col];
              break;
            }
          }
          if (_buffer.data[_trace.col] == '\\' && _trace.col + 1 < _buffer.data.size()) {
            if (check_for_chars(value, _buffer.data[_trace.col + 1])) {
              _trace.col += 2;
              continue;
            }
          }
          value += _buffer.data[_trace.col++];
        }
        if (!value.ends_with('"')) {
          emit_error("Unterminated string");
          return {};
        }
        value = value.substr(1, value.size() - 2);
        list->push_back(
          std::make_unique<atom_string_c>(
            value, file_position_s{_trace.line, start}));
        break;
      }

    default: {
          std::size_t start = _trace.col;
          if (std::isdigit(_buffer.data[_trace.col])) {
           std::string number;
           number += _buffer.data[_trace.col];
           while (!at_end() &&
                 (std::isdigit(_buffer.data[_trace.col + 1]) || 
                  _buffer.data[_trace.col + 1] == '.')) {
             number += _buffer.data[_trace.col + 1];
             _trace.col++;
           }

           if (std::regex_match(number, is_number)) {
             if (number.find('.') != std::string::npos) {
                list->push_back(
                  std::make_unique<atom_real_c>(
                    std::stod(number), file_position_s{_trace.line, start}));
             } else {
                list->push_back(
                  std::make_unique<atom_int_c>(
                    std::stoll(number), file_position_s{_trace.line, start}));
             }
             break;
           } else {
             emit_error("Malformed numerical value");
             return {};
           }
           break;
         }

         std::string word;
         word += _buffer.data[_trace.col];

         while (!at_end() &&
                !std::isspace(_buffer.data[_trace.col + 1]) &&
                (!_termChars.contains(_buffer.data[_trace.col + 1]))) {

           word += _buffer.data[_trace.col + 1];
           _trace.col++;
         }
         list->push_back(
           std::make_unique<atom_symbol_c>(
             word, file_position_s{_trace.line, start}));
         break;
      }
    }
    _trace.col++;
  }

  if (list) { return std::move(*list); }
  return {};
}

void atomiser_c::emit_error(const std::string &err) {
  _receiver.on_error(_trace.line, _trace.col, err);
  reset();
}

void atomiser_c::reset() {
  _trace = {0,0};
  _buffer = {0, ""};
}

