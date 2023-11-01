#pragma once

#include <functional>
#include <optional>
#include <memory>
#include <string>
#include <stack>
#include <vector>

namespace parser {

enum class atom_type_e {
  UNDEFINED = 0,
  SYMBOL,       // Symbols are some chunk of text, use meta_e to classify 
  INTEGER,      // Note: We don't care about size of integers in the parser
  REAL,
  STRING,
  CHAR,
  LOAD_ARG,    // Pull result of previous computation as an argument
};

enum class meta_e {
  UNDEFINED,
  IDENTIFIER,
  TILDE, BACK_TICK,
  EXCLAMATION_POINT,
  AT, DOLLAR, MOD, HAT, AMPERSAND,
  ASTERISK, SUB, PLUS, EQUAL, NOT_EQUAL,
  LEFT_CURLY, RIGHT_CURLY, LEFT_BRACKET, RIGHT_BRACKET,
  PIPE, OR, COLON, COMMA, PERIOD,
  LT, GT, LTE, GTE, LSH, RSH, QUESTION_MARK, FORWARD_SLASH,
  SCOPE,
  EQUAL_EQUAL,
};

class atom_c {
public:
  atom_c(const atom_type_e atom_type,
              const meta_e meta,
              std::string data,
              const size_t line,
              const size_t col)
    : line(line),
      col(col),
      atom_type(atom_type),
      meta(meta),
      data(data) {}
  size_t line{0};
  size_t col{0};
  atom_type_e atom_type{atom_type_e::UNDEFINED};
  meta_e meta{meta_e::UNDEFINED};
  std::string data;
};

struct error_s {
  std::string message;
  size_t line{0};
  size_t col{0};
};

using list_t = std::vector<std::unique_ptr<atom_c>>;
using error_cb_f = std::function<void(error_s)>;
using list_cb_f = std::function<void(list_t)>;

extern void print_list(list_t& list);

//! \class parser_c
//! \brief A parser that allows data to be submitted to
//!        it in pieces. When a list is detected/parsed
//!        a callback will fire handing the list over.
//!        In the event of an error, an error cb is fired.
//! \note This parser will return lists as they complete, 
//!       so that the inner-most lists will be emitted first
//  
//        (+ 1 2 (/ 1 2))   ->    (/ 1 2) (+ 1 2 LOAD_ARG)
//
//
class parser_c {
public:
  parser_c() = delete;
  parser_c(list_cb_f list_cb, error_cb_f error_cb)
    : _list_cb(list_cb), _error_cb(error_cb) {}

  //! \brief Submit some string data to the parser.
  //!        If the parser detects a fully processed list,
  //!        the parser will hand the list via the callback.
  //!        If no full list is detected, the input will be buffered
  //!        and a subsequent `submit` will continue the processing.
  void submit(const char* data, size_t line=0);
  void submit(std::string &data, size_t line=0);

  //! \brief Indicate that the parsing is finished.
  //!        In the event that there is incomplete
  //!        data in the buffer, an error will be fired.
  void finish();

private:
  list_cb_f _list_cb{nullptr};
  error_cb_f _error_cb{nullptr};

  struct {
    size_t pdepth{0};
    size_t line{0};
    size_t col{0};
  } _trace;

  void insert_atom(list_t *list,
    const atom_type_e,
    const meta_e,
    const std::string&);
  void parse(list_t* list, std::string &string_data);
  void emit_error(const std::string&);
  void reset();

  std::stack<list_t> _active_lists;
};

} // namespace

