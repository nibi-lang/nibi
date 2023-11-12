#include "lexer.hpp"

#include <fmt/format.h>
#include <algorithm>
#include <cctype>
#include <map>
#include <regex>

namespace front {

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

} // namespace

void print_list(front::atom_list_t &list) {
  for(auto &atom : list) {
    fmt::print("ATOM ({}:{})[{}]", atom->pos.line, atom->pos.col, atom->data);
  }
  fmt::print("\n");
}

void lexer_c::insert_atom(
  atom_list_t *list,
  const atom_type_e type,
  const meta_e meta,
  const std::string &data) {
    if (!list) {
      emit_error("Attempt to add item outside of list");
      return;
    }
    list->push_back(
      std::make_unique<atom_c>(
          type,
          meta,
          data,
          _trace.line,
          _trace.col));
}

void lexer_c::submit(const char* data, size_t line) {
  if (!data) {
    return;
  }
  std::string val = data;
  return submit(val, line);
}

void lexer_c::submit(std::string &data, size_t line) {

  if (data.empty())
    return;

  _trace.line = line;
  _trace.col = 0;

  if (_active_lists.empty()) {
    parse(nullptr, data);
  } else {
    parse(&_active_lists.top(), data);
  }

  if (_active_lists.empty()) {
    _receiver.on_top_list_complete();
  }
}

bool lexer_c::finish() {
  if (!_active_lists.empty()) {
    emit_error("Incomplete list - Finish indicated with data buffered");
    return false;
  }
  return true;
}

#define ADD_SYM(sym, meta) \
  case sym: {\
  std::string buff; buff += sym; \
  insert_atom(list, atom_type_e::SYMBOL, meta, buff); \
  buff.clear(); \
  break;}

#define ADD_DOUBLE_SYM(syml, symr, metal, metar) \
  case syml: {\
    std::string buff; buff += syml; \
    if (_trace.col + 1 < line_data.size() && line_data[_trace.col+1] == symr) { \
      _trace.col++; \
      buff = buff + line_data[_trace.col]; \
      insert_atom(list, atom_type_e::SYMBOL, metar, buff); \
    } else {\
      insert_atom(list, atom_type_e::SYMBOL, metal, buff); \
    } \
    buff.clear(); \
    break;}

#define ADD_TRIPPLE_SYM(syml, symm, symr, metal, metam, metar) \
  case syml: {\
    std::string buff; buff += syml; \
    if (_trace.col + 1 < line_data.size() && line_data[_trace.col+1] == symm) { \
      _trace.col++; \
      buff = buff + line_data[_trace.col]; \
      insert_atom(list, atom_type_e::SYMBOL, metam, buff); \
    } else if (_trace.col + 1 < line_data.size() && line_data[_trace.col+1] == symr) { \
      _trace.col++; \
      buff = buff + line_data[_trace.col]; \
      insert_atom(list, atom_type_e::SYMBOL, metar, buff); \
    } else {\
      insert_atom(list, atom_type_e::SYMBOL, metal, buff); \
    } \
    break;}

void lexer_c::parse(atom_list_t *list, std::string &line_data) {

  while(_trace.col < line_data.size()) {

    auto c = line_data[_trace.col];

    if (std::isspace(c)) {
      _trace.col++;
      continue;
    }

    switch (c) {
      ADD_SYM('\'', meta_e::SINGLE_QUOTE)
      ADD_SYM('~', meta_e::TILDE)
      ADD_SYM('`', meta_e::BACK_TICK)
      ADD_SYM('@', meta_e::AT)
      ADD_SYM('$', meta_e::DOLLAR)
      ADD_SYM('%', meta_e::MOD)
      ADD_SYM('^', meta_e::HAT)
      ADD_SYM('&', meta_e::AMPERSAND)
      ADD_SYM('*', meta_e::ASTERISK)
      ADD_SYM('+', meta_e::PLUS)
      ADD_SYM('{', meta_e::LEFT_CURLY)
      ADD_SYM('}', meta_e::RIGHT_CURLY)
      ADD_SYM('[', meta_e::LEFT_BRACKET)
      ADD_SYM(']', meta_e::RIGHT_BRACKET)
      ADD_SYM('?', meta_e::QUESTION_MARK)
      ADD_SYM('/', meta_e::FORWARD_SLASH)
      ADD_DOUBLE_SYM('=', '=', meta_e::EQUAL, meta_e::EQUAL_EQUAL);
      ADD_DOUBLE_SYM('!', '=', meta_e::EXCLAMATION_POINT, meta_e::NOT_EQUAL);
      ADD_DOUBLE_SYM('|', '|', meta_e::PIPE, meta_e::OR);
      ADD_DOUBLE_SYM('>', '>', meta_e::GT, meta_e::RSH);
      ADD_TRIPPLE_SYM('<', '-', '<', meta_e::LT, meta_e::LEFT_ARROW, meta_e::LSH);

      case '(': {
        size_t line = _trace.line;
        size_t col = _trace.col;

        _trace.pdepth++;
        _trace.col++;

        // If we are already in a list we need to identify
        // that they should pull the result of the next as 
        // an input arg
        if (list) {
          list->push_back(
            std::make_unique<atom_c>(
                atom_type_e::LOAD_ARG,
                meta_e::UNDEFINED,
                "<load arg>",
                line,
                col));
        }

        _active_lists.push({});
        parse(&_active_lists.top(), line_data);
        break;
      }

      case ')': {
        if (_trace.pdepth == 0) {
          emit_error("Unmatch termination symbol ')'");
          return;
        }

        _trace.pdepth--;

        _receiver.on_list(std::move(*list));
        _active_lists.pop();

        if (_active_lists.size()) {
          list = &_active_lists.top();
        }
        break;
      }

      case '#':
        [[fallthrough]];
      case ';': {
        _trace.col = line_data.size();
        return;
      }

      case '"': {
        bool in_str{true};
        std::string value = "\"";
        decltype(_trace.col) start = _trace.col++;
        while (_trace.col < line_data.size()) {
         if (line_data[_trace.col] == '"') {
            if (_trace.col > 0 && line_data[_trace.col - 1] != '\\') {
              value += line_data[_trace.col];
              break;
            }
          }
          if (line_data[_trace.col] == '\\' && _trace.col + 1 < line_data.size()) {
            if (check_for_chars(value, line_data[_trace.col + 1])) {
              _trace.col += 2;
              continue;
            }
          }
          value += line_data[_trace.col++];
        }
        if (!value.ends_with('"')) {
          emit_error("Unterminated string");
          return;
        }
        value = value.substr(1, value.size() - 2);
        insert_atom(list, atom_type_e::STRING, meta_e::UNDEFINED, value);
        break;
      }

    default: {
         if (std::isdigit(line_data[_trace.col]) || line_data[_trace.col] == '-') {
           if (line_data[_trace.col] == '-' && _trace.col + 1 < line_data.size() &&
               !std::isdigit(line_data[_trace.col + 1])) {
             insert_atom(list, atom_type_e::SYMBOL, meta_e::SUB, "-"); \
             break;
           } else if (line_data[_trace.col] == '-' && _trace.col == line_data.size() - 1) {
             insert_atom(list, atom_type_e::SYMBOL, meta_e::SUB, "-"); \
             break;
           }

           std::string number;
           number += line_data[_trace.col];
           while (_trace.col + 1 < line_data.size() &&
                  (std::isdigit(line_data[_trace.col + 1]) || line_data[_trace.col + 1] == '.')) {
             number += line_data[_trace.col + 1];
             _trace.col++;
           }

           if (std::regex_match(number, is_number)) {
             if (number.find('.') != std::string::npos) {
               insert_atom(list, atom_type_e::REAL, meta_e::UNDEFINED, number); \
             } else {
               insert_atom(list, atom_type_e::INTEGER, meta_e::UNDEFINED, number); \
             }
             break;
           } else {
             emit_error("Malformed numerical value");
             return;
           }
           break;
         }

         std::string word;
         word += line_data[_trace.col];

         auto meta_type = meta_e::IDENTIFIER;
         while (_trace.col + 1 < line_data.size() && !std::isspace(line_data[_trace.col + 1]) &&
                line_data[_trace.col + 1] != '(' && line_data[_trace.col + 1] != ')' &&
                line_data[_trace.col + 1] != '[' && line_data[_trace.col + 1] != ']' &&
                line_data[_trace.col + 1] != '{' && line_data[_trace.col + 1] != '}') {

           word += line_data[_trace.col + 1];
           if (line_data[_trace.col + 1] == '.') {
             meta_type = meta_e::ACCESSOR;
           }
           _trace.col++;
         }

         if (word[0] == ':') {
            insert_atom(list, atom_type_e::SYMBOL, meta_e::TAG, word);
            break;
         }

         insert_atom(list, atom_type_e::SYMBOL, meta_type, word);
         break;
      }
    }
    _trace.col++;
  }
}

void lexer_c::emit_error(const std::string &err) {
  error_s e{err, {_trace.line, _trace.col}};
  _receiver.on_error(e);
  reset();
}

void lexer_c::reset() {
  _trace = {0,0,0};
  _active_lists = {};
}

} // namespace

