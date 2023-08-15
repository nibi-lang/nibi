#pragma once

#include "error.hpp"
#include "interpreter/interpreter.hpp"
#include <iterator>

namespace nibi {

#define UNUSED(expr)                                                           \
  do {                                                                         \
    (void)(expr);                                                              \
  } while (0)

#define NIBI_VALIDATE_VAR_NAME(___name, ___loc)                                \
  if (___name[0] == '$' || ___name[0] == ':') {                                \
    throw interpreter_c::exception_c(                                          \
        "Cannot assign to a variable starting with '$' or ':'", ___loc);       \
  }

// Iterate over a list, executing the loop body for each element
// after skipping the first n elements, and loading the value
// into an "arg" variable
#define NIBI_LIST_ITER_AND_LOAD_SKIP_N(___n, ___loop_body)                     \
  for (auto i = std::next(list.begin(), ___n); i != list.end(); ++i) {         \
    nibi::cell_ptr arg = ci.process_cell(*i, env);                             \
    ___loop_body                                                               \
  }

// Check if the list is a given size, and if not error out.
// converts size to number of arguments expected, so if you want 2 arguments
// you should pass 3
#define NIBI_LIST_ENFORCE_SIZE(___cmd, ___op, ___size)                         \
  if (!(list.size() ___op ___size)) {                                          \
    throw nibi::interpreter_c::exception_c(                                    \
        std::string(___cmd) + " instruction expects " +                        \
            std::to_string(___size - 1) + " parameters, got " +                \
            std::to_string(list.size() - 1) + ".",                             \
        list.front()->locator);                                                \
    return nibi::allocate_cell(nibi::cell_type_e::NIL);                        \
  }
} // namespace nibi
