#pragma once

#include <functional>
#include <optional>
#include <memory>
#include <string>
#include <vector>

namespace parser {

enum class node_type_e {
  UNDEFINED = 0,
  LIST,
  ATOM
};

class node_c {
public:
  node_c() = delete;
  node_c(const node_type_e node_type,
         const size_t line,
         const size_t col)
    : node_type(node_type),
      line(line),
      col(col){}
  node_type_e node_type{node_type_e::UNDEFINED};
  size_t line{0};
  size_t col{0};
};

using list_t = std::vector<std::unique_ptr<node_c>>;

class list_node_c : public node_c {
public:
  list_node_c()
    : node_c(node_type_e::LIST, 0, 0) {}
  list_node_c(list_t data, const size_t line, const size_t col)
    : node_c(node_type_e::LIST, line, col),
      list_data(std::move(data)){}
  list_t list_data;
};

enum class atom_type_e {
  UNDEFINED = 0,
  INTEGER,
  REAL,
  STRING,
  SYMBOL,
};

enum class meta_e {
  IDENTIFIER,
  COMMENT,            // Following first instance of ';'
  DIRECTIVE,          // Lines starting with '#'
  TILDE, BACK_TICK,
  EXCLAMATION_POINT,
  AT, DOLLAR, MOD, HAT, AMPERSAND,
  ASTERISK, SUB, UNDERSCORE, PLUS, EQUAL, NOT_EQUAL,
  LEFT_CURLY, RIGHT_CURLY, LEFT_BRACKET, RIGHT_BRACKET,
  PIPE, OR, COLON, COMMA, PERIOD,
  LT, GT, LTE, GTE, LSH, RSH, QUESTION_MARK, FORWARD_SLASH,
  SCOPE,
  EQUAL_EQUAL,
};

class atom_node_c final : public node_c {
public:
  atom_node_c(const atom_type_e atom_type,
              const meta_e meta,
              std::string data,
              const size_t line,
              const size_t col)
    : node_c(node_type_e::ATOM, line, col),
      atom_type(atom_type),
      meta(meta),
      atom_data(data) {}
  atom_type_e atom_type{atom_type_e::UNDEFINED};
  meta_e meta{meta_e::IDENTIFIER};
  std::string atom_data;
};

struct error_s {
  std::string message;
  size_t line{0};
  size_t col{0};
};

using error_cb_f = std::function<void(error_s)>;
using list_cb_f = std::function<void(list_t)>;

static inline atom_node_c* node_as_atom(std::unique_ptr<node_c> &n) {
  return static_cast<atom_node_c*>(n.get());
}

static inline list_t* node_as_list(std::unique_ptr<node_c> &n) {
  return &(static_cast<list_node_c*>(n.get())->list_data);
}

extern void print_list(list_t& list);

//! \class parser_c
//! \brief A parser that allows data to be submitted to
//!        it in pieces. When a list is detected/parsed
//!        a callback will fire handing the list over.
//!        In the event of an error, an error cb is fired.
class parser_c {
public:
  parser_c() = delete;
  parser_c(list_cb_f list_cb, error_cb_f error_cb)
    : _list_cb(list_cb), _error_cb(error_cb) {}

  //! \brief Submit some string data to the parser.
  //!        If the parser detects a fully processed list,
  //!        the parser will hand the list via the callback.
  void submit(std::string &data);

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

  std::string _line_buffer;
  list_t _partial_list;

  void insert_atom(list_t *list,
    const atom_type_e,
    const meta_e,
    std::string&);
  void parse(list_t* list);
  void emit_error(const std::string&);
  void reset();
};

extern void test();

} // namespace

