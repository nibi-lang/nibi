#pragma once

#include "libnibi/parallel_hashmap/phmap.hpp"
#include <memory>
#include <string>

// Forward declaration for RLL library loader
namespace rll {
class shared_library;
}

namespace nibi {

//! \brief A wrapper fo the rll lib as the header can only be included
//!        once in a source file due to design decisions of the authors
class rll_wrapper_c {
public:
  //! \brief The definition of the thrown errors also needs
  //         to be hidden as it would cause include issues so
  //         we need to wrap that too
  class library_loading_error_c : public std::exception {
  public:
    library_loading_error_c() = delete;
    library_loading_error_c(const char *what) : _what(what) {}
    const char *what() const throw() { return _what; }

  private:
    const char *_what;
  };

  //! \brief Create the wrapper
  rll_wrapper_c();

  //! \brief wrap rll::shared_library::load
  void load(std::string target);

  //! \brief wrap rll::shared_library::is_loaded
  bool is_loaded();

  //! \brief wrap rll::shared_library::has_symbol
  bool has_symbol(std::string symbol);

  //! \brief wrap rll::shared_library::get_symbol
  void *get_symbol(std::string symbol);

private:
  std::shared_ptr<rll::shared_library> _lib;
};

using rll_ptr = std::shared_ptr<rll_wrapper_c>;

constexpr auto allocate_rll = [](auto... args) -> nibi::rll_ptr {
  return std::make_shared<nibi::rll_wrapper_c>(args...);
};

using rll_map = phmap::parallel_node_hash_map<std::string, rll_ptr>;
} // namespace nibi
