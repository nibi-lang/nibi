#pragma once

#include <stack>

#include "lang/atoms.hpp"
#include <set>

//! \class atomiser_c
//! \brief A parser that allows data to be submitted to
//!        it in pieces. When a list is detected/parsed
//!        a callback will fire handing the list over.
//!        In the event of an error, an error cb is fired.
//! \note This parser will return lists as they complete, 
//!       so that the inner-most lists will be emitted first
//  
//        (+ 1 2 (/ 1 2))   ->    (/ 1 2) (+ 1 2 LOAD_ARG)
//
class atom_receiver_if {
public:
  virtual void on_error(
    const std::size_t line,
    const std::size_t& col,
    const std::string& message) = 0;

  virtual void on_list(atom_list_t list) = 0;
};

class atomiser_c {
public:
  atomiser_c() = delete;
  atomiser_c(atom_receiver_if &receiver);

  //! \brief Submit some string data to the parser.
  //!        If the parser detects a fully processed list,
  //!        the parser will hand the list via the callback.
  //!        If no full list is detected, the input will be buffered
  //!        and a subsequent `submit` will continue the processing.
  //! \note  The submission can take multiple statements and will
  //!        trigger the atom_receiver for every top-level list that
  //!        is found as long as they are fully formed.
  void submit(const char* data, size_t line=0);
  void submit(std::string &data, size_t line=0);

  //! \brief Indicate that the atomisation is completed. If the
  //!        atomiser still containes buffered data it will fire
  //!        an error indicating that there remains an open list
  void indicate_complete();

  void reset();
private:
  atom_receiver_if &_receiver;

  struct {
    std::size_t line{0};
    std::size_t col{0};
  } _trace;

  struct buffer_s {
    std::size_t tracker{0};
    std::string data;

    [[nodiscard]] bool ready() {
      return (tracker == 0 && data.size());
    }
  };

  buffer_s _buffer;

  void add_to_buffer(const std::string&);
  atom_list_t parse(atom_list_t* list);
  void emit_error(const std::string&);

  const bool at_end() {
    return (_trace.col + 1 >= _buffer.data.size());
  }

  std::set<uint8_t> _termChars;
};

