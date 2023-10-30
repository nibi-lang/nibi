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
  node_c(const node_type_e node_type)
    : node_type(node_type) {}
  node_type_e node_type{node_type_e::UNDEFINED};
};

using list_t = std::vector<std::unique_ptr<node_c>>;

class list_node_c : public node_c {
public:
  list_node_c()
    : node_c(node_type_e::LIST) {}
  list_node_c(list_t data)
    : node_c(node_type_e::LIST),
      data(std::move(data)){}
  list_t data;
};

enum class atom_type_e {
  UNDEFINED = 0,
  INTEGER,
  REAL,
  STRING,
  SYMBOL
};

class atom_node_c final : public node_c {
public:
  atom_node_c(const atom_type_e atom_type, 
              const std::string& data)
    : node_c(node_type_e::ATOM),
      atom_type(atom_type),
      data(data) {}
  atom_type_e atom_type{atom_type_e::UNDEFINED};
  std::string data;
};

struct error_s {
  std::string message;
  size_t line{0};
  size_t col{0};
};

using error_cb_f = std::function<void(error_s)>;
using list_cb_f = std::function<void(list_t)>;

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
  void submit(const std::string &data);

  //! \brief Indicate that the parsing is finished.
  //!        In the event that there is incomplete
  //!        data in the buffer, an error will be fired.
  void finish();

private:
  list_cb_f _list_cb{nullptr};
  error_cb_f _error_cb{nullptr};
  size_t _tracker{0};
  std::string _buffer;
};

extern void test();

} // namespace

