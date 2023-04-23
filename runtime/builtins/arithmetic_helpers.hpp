#pragma once
#include "cpp_macros.hpp"

template<typename T>
static inline T list_perform_sum(T base_value, std::function<T(cell_c*)> conversion_method, cell_list_t& list, env_c &env) {
  T accumulate{base_value};
  LIST_ITER_AND_LOAD_SKIP_N(2, {
    accumulate += conversion_method(arg);
  })
  return accumulate;
}

