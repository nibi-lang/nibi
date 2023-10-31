#include "parser/parser.hpp"

#include <iostream>
#include <algorithm>
#include <stack>

namespace parser {

namespace {

static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

static inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

} // namespace

void print_list(parser::list_t &list) {
  for(auto &atom : list) {
    std::cout << " ATOM (" << atom->line << ":" << atom->col << ")[" << atom->data << "]";
  }
  std::cout << std::endl;
}

void parser_c::insert_atom(
  list_t *list,
  const atom_type_e type,
  const meta_e meta,
  std::string &data) {
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

void parser_c::submit(std::string &data) {

 // trim(data); // This will much with cols

  if (data.empty())
    return;

  if (_line_buffer.empty() && data[0] == '#') {
    list_t list;
    insert_atom(&list, atom_type_e::SYMBOL, meta_e::DIRECTIVE, data);
    _list_cb(std::move(list));
    return;
  }

  if (_line_buffer.empty() && data[0] == ';') {
    list_t list;
    insert_atom(&list, atom_type_e::SYMBOL, meta_e::COMMENT, data);
    _list_cb(std::move(list));
    return;
  }

  _line_buffer += data;
  _trace.line++;

  if (_partial_list.empty()) {
    parse(nullptr);
  } else {
    parse(&_partial_list);
  }
}

void parser_c::finish() {

}

#define ADD_SYM(sym, meta) \
  case sym:\
  insert_atom(list, atom_type_e::SYMBOL, meta, buff); \
  buff.clear(); \
  break;

#define ADD_DOUBLE_SYM(syml, symr, metal, metar) \
  case syml: \
    if (_trace.col + 1 < _line_buffer.size() && _line_buffer[_trace.col+1] == symr) { \
      _trace.col++; \
      buff = buff + _line_buffer[_trace.col]; \
      insert_atom(list, atom_type_e::SYMBOL, metar, buff); \
    } else {\
      insert_atom(list, atom_type_e::SYMBOL, metal, buff); \
    } \
    buff.clear(); \
    break;

#define ADD_TRIPPLE_SYM(syml, symm, symr, metal, metam, metar) \
  case syml: \
    if (_trace.col + 1 < _line_buffer.size() && _line_buffer[_trace.col+1] == symm) { \
      _trace.col++; \
      buff = buff + _line_buffer[_trace.col]; \
      insert_atom(list, atom_type_e::SYMBOL, metam, buff); \
    } else if (_trace.col + 1 < _line_buffer.size() && _line_buffer[_trace.col+1] == symr) { \
      _trace.col++; \
      buff = buff + _line_buffer[_trace.col]; \
      insert_atom(list, atom_type_e::SYMBOL, metar, buff); \
    } else {\
      insert_atom(list, atom_type_e::SYMBOL, metal, buff); \
    } \
    buff.clear(); \
    break;

void parser_c::parse(list_t *list) {

  std::string buff;
  while(_trace.col < _line_buffer.size()) {

    auto c = _line_buffer[_trace.col];

    if (std::isspace(c)) {
      _trace.col++;
      continue;
    }

    buff = buff + c;
    switch (c) {
      ADD_SYM('~', meta_e::TILDE)
      ADD_SYM('`', meta_e::BACK_TICK)
      ADD_SYM('@', meta_e::AT)
      ADD_SYM('$', meta_e::DOLLAR)
      ADD_SYM('%', meta_e::MOD)
      ADD_SYM('^', meta_e::HAT)
      ADD_SYM('&', meta_e::AMPERSAND)
      ADD_SYM('*', meta_e::ASTERISK)
      ADD_SYM('-', meta_e::SUB)
      ADD_SYM('_', meta_e::UNDERSCORE)
      ADD_SYM('+', meta_e::PLUS)
      ADD_SYM('{', meta_e::LEFT_CURLY)
      ADD_SYM('}', meta_e::RIGHT_CURLY)
      ADD_SYM('[', meta_e::LEFT_BRACKET)
      ADD_SYM(']', meta_e::RIGHT_BRACKET)
      ADD_SYM(',', meta_e::COMMA)
      ADD_SYM('.', meta_e::PERIOD)
      ADD_SYM('?', meta_e::QUESTION_MARK)
      ADD_SYM('/', meta_e::FORWARD_SLASH)
      ADD_DOUBLE_SYM('=', '=', meta_e::EQUAL, meta_e::EQUAL_EQUAL);
      ADD_DOUBLE_SYM('!', '=', meta_e::EXCLAMATION_POINT, meta_e::NOT_EQUAL);
      ADD_DOUBLE_SYM(':', ':', meta_e::COLON, meta_e::SCOPE);
      ADD_DOUBLE_SYM('|', '|', meta_e::PIPE, meta_e::OR);
      ADD_TRIPPLE_SYM('<', '=', '<', meta_e::LT, meta_e::LTE, meta_e::LSH);
      ADD_TRIPPLE_SYM('>', '=', '>', meta_e::GT, meta_e::GTE, meta_e::RSH);

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

        list_t new_list;
        parse(&new_list);

        buff.pop_back();
        break;
      }

      case ')': {
        if (_trace.pdepth == 0) {
          emit_error("Unmatch termination symbol ')'");
          return;
        }

        _trace.pdepth--;
        _trace.col++;

        _list_cb(std::move(*list));
        return;
      }

      case ';': {
        // Eat the rest of the line as a comment and add to list. 
        // Return without emitting
      break;
      }
    };

    _trace.col++;
  }

}

void parser_c::emit_error(const std::string &err) {
  error_s e{err, _trace.line, _trace.col};
  _error_cb(e);
  reset();
}

void parser_c::reset() {
  _trace = {0,0,0};
  _line_buffer.clear();
  _partial_list.clear();
}

} // namespace

