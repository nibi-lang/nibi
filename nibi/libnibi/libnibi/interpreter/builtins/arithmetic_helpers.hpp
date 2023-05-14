#pragma once

#include "cpp_macros.hpp"
#include <iostream>
#include <math.h>

namespace nibi {

template <typename T>
static inline T list_perform_add(T base_value, interpreter_c &ci,
                                 std::function<T(cell_ptr)> conversion_method,
                                 cell_list_t &list, env_c &env) {
  T accumulate{base_value};
  LIST_ITER_AND_LOAD_SKIP_N(2, { accumulate += conversion_method(arg); })
  return accumulate;
}

template <typename T>
static inline T list_perform_sub(T base_value, interpreter_c &ci,
                                 std::function<T(cell_ptr)> conversion_method,
                                 cell_list_t &list, env_c &env) {
  T accumulate{base_value};
  LIST_ITER_AND_LOAD_SKIP_N(2, { accumulate -= conversion_method(arg); })
  return accumulate;
}

template <typename T>
static inline T list_perform_div(T base_value, interpreter_c &ci,
                                 std::function<T(cell_ptr)> conversion_method,
                                 cell_list_t &list, env_c &env) {
  T accumulate{base_value};
  LIST_ITER_AND_LOAD_SKIP_N(2, {
    auto r = conversion_method(arg);
    if (r == 0) {
      throw interpreter_c::exception_c("Division by zero", arg->locator);
      return accumulate;
    }
    accumulate /= r;
  })
  return accumulate;
}

template <typename T>
static inline T list_perform_mul(T base_value, interpreter_c &ci,
                                 std::function<T(cell_ptr)> conversion_method,
                                 cell_list_t &list, env_c &env) {
  T accumulate{base_value};
  LIST_ITER_AND_LOAD_SKIP_N(2, { accumulate *= conversion_method(arg); })
  return accumulate;
}

template <typename T>
static inline T list_perform_pow(T base_value, interpreter_c &ci,
                                 std::function<T(cell_ptr)> conversion_method,
                                 cell_list_t &list, env_c &env) {
  T accumulate{base_value};
  LIST_ITER_AND_LOAD_SKIP_N(
      2, { accumulate = std::pow(accumulate, conversion_method(arg)); })
  return accumulate;
}

} // namespace nibi