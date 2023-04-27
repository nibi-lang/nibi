#pragma once

#include "runtime/runtime.hpp"
#include <iterator>

#define LIST_ITER_SKIP_N(___n, ___loop_body)                                   \
  auto it = list.begin();                                                      \
  std::advance(it, ___n);                                                      \
  for (; it != list.end(); ++it)                                               \
  ___loop_body

#define LIST_ITER_AND_LOAD_SKIP_N(___n, ___loop_body)                          \
  auto it = list.begin();                                                      \
  std::advance(it, ___n);                                                      \
  for (; it != list.end(); ++it) {                                             \
    cell_c *arg = global_runtime->execute_cell(*it, env);                      \
    ___loop_body                                                               \
  }

// Check if the list is a given size, and if not error out.
// converts size to number of arguments expected, so if you want 2 arguments
// you should pass 3
#define LIST_ENFORCE_SIZE(___cmd, ___op, ___size)                              \
  if (!(list.size() ___op ___size)) {                                          \
    static_assert(___size > 0, "LIST_ENFORCE_SIZE must be > 0");               \
    global_runtime->halt_with_error(                                           \
        error_c(list.front()->locator,                                         \
                std::string(___cmd) + " instruction expects " +                \
                    std::to_string(___size - 1) + " parameters, got " +        \
                    std::to_string(list.size() - 1) + "."));                   \
    return global_cell_nil;                                                    \
  }

//! \brief Get the nth argument from a list of cells from an execution list,
//!        and execute it with the runtime to load the target value
//! \warning This function does not check to ensure that the list is long enough
//!          to contain the nth argument.  It is the responsibility of the
//!          caller
static inline cell_c *list_get_nth_arg(std::size_t n, cell_list_t &list,
                                       env_c &env) {
  auto it = list.begin();
  std::advance(it, n);
  return global_runtime->execute_cell(*it, env);
}
