#pragma once

#include "cpp_macros.hpp"

#include <math.h>

template <typename T>
static inline T list_perform_add(T base_value,
                                 std::function<T(cell_c *)> conversion_method,
                                 cell_list_t &list, env_c &env) {
  T accumulate{base_value};
  LIST_ITER_AND_LOAD_SKIP_N(2, { accumulate += conversion_method(arg); })
  return accumulate;
}

template <typename T>
static inline T list_perform_sub(T base_value,
                                 std::function<T(cell_c *)> conversion_method,
                                 cell_list_t &list, env_c &env) {
  T accumulate{base_value};
  LIST_ITER_AND_LOAD_SKIP_N(2, { accumulate -= conversion_method(arg); })
  return accumulate;
}

template <typename T>
static inline T list_perform_div(T base_value,
                                 std::function<T(cell_c *)> conversion_method,
                                 cell_list_t &list, env_c &env) {
  T accumulate{base_value};
  LIST_ITER_AND_LOAD_SKIP_N(2, {
    auto r = conversion_method(arg);
    if (r == 0) {
      global_runtime->halt_with_error(
          error_c(arg->locator, "Division by zero"));
    }
    accumulate /= r;
  })
  return accumulate;
}

template <typename T>
static inline T list_perform_mul(T base_value,
                                 std::function<T(cell_c *)> conversion_method,
                                 cell_list_t &list, env_c &env) {
  T accumulate{base_value};
  LIST_ITER_AND_LOAD_SKIP_N(2, { accumulate *= conversion_method(arg); })
  return accumulate;
}

template <typename T>
static inline T list_perform_pow(T base_value,
                                 std::function<T(cell_c *)> conversion_method,
                                 cell_list_t &list, env_c &env) {
  T accumulate{base_value};
  LIST_ITER_AND_LOAD_SKIP_N(
      2, { accumulate = std::pow(accumulate, conversion_method(arg)); })
  return accumulate;
}
