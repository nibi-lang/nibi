#pragma once

#include <stack>

#include "atoms.hpp"
#include <set>

namespace front {

//! \class lexer_c
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
class lexer_c {
public:
  lexer_c() = delete;
  lexer_c(front::atom_receiver_if &receiver);

  //! \brief Submit some string data to the parser.
  //!        If the parser detects a fully processed list,
  //!        the parser will hand the list via the callback.
  //!        If no full list is detected, the input will be buffered
  //!        and a subsequent `submit` will continue the processing.
  //! \note When the parser detects the completion of a top-most list
  //!       the lfin_f callback will be fired to indicate so
  void submit(const char* data, size_t line=0);
  void submit(std::string &data, size_t line=0);

  //! \brief Indicate that the parsing is finished.
  //!        In the event that there is incomplete
  //!        data in the buffer, an error will be fired.
  bool finish();

private:
  front::atom_receiver_if &_receiver;

  struct {
    size_t pdepth{0};
    size_t line{0};
    size_t col{0};
  } _trace;

  void insert_atom(atom_list_t *list,
    const atom_type_e,
    const meta_e,
    const std::string&);
  void parse(atom_list_t* list, std::string &string_data);
  void emit_error(const std::string&);
  void reset();

  std::stack<atom_list_t> _active_lists;
  std::set<uint8_t> _termChars;
};

} // namespace

