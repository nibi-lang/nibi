#include "lib.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <random>
#include <filesystem>
#include <limits>
#include <libnibi/common/platform.hpp>

namespace {
template <typename T> class generate_random_c {
public:
  explicit generate_random_c() : eng(rd()) {}

  T get_range(T min, T max)
  {
    std::uniform_int_distribution<T> dist(min, max);
    return dist(eng);
  }

  T get_floating_point_range(T min, T max)
  {
    std::uniform_real_distribution<T> dist(min, max);
    return dist(eng);
  }

private:
  std::random_device rd;
  std::default_random_engine eng;
};
}

#define CHECK_SIZE(op, n)                                                      \
  if (!(list.size() op n)) {                                                      \
    ci.halt_with_error(nibi::error_c(                                          \
        list.front()->locator,                                                 \
        "instruction expects " + std::to_string(n - 1) + " parameters, got " + \
            std::to_string(list.size() - 1) + "."));                           \
  }

nibi::cell_ptr rand_int(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                        nibi::env_c &env) {
  return nibi::allocate_cell(
    (int64_t)generate_random_c<int64_t>().get_range(
      std::numeric_limits<int64_t>::min(),
      std::numeric_limits<int64_t>::max()));
}

nibi::cell_ptr rand_double(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                        nibi::env_c &env) {
  return nibi::allocate_cell(
    (double)generate_random_c<double>().get_floating_point_range(
      std::numeric_limits<double>::min(),
      std::numeric_limits<double>::max()));
}

nibi::cell_ptr rand_range_int(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                        nibi::env_c &env) {
  CHECK_SIZE(==, 3)
  auto min = ci.execute_cell(list[1], env);
  auto max = ci.execute_cell(list[2], env);
  return nibi::allocate_cell(
    (int64_t)generate_random_c<int64_t>().get_range(
      min->to_integer(),
      max->to_integer()));
}

nibi::cell_ptr rand_range_double(nibi::interpreter_c &ci, nibi::cell_list_t &list,
                        nibi::env_c &env) {
  CHECK_SIZE(==, 3)
  auto min = ci.execute_cell(list[1], env);
  auto max = ci.execute_cell(list[2], env);
  return nibi::allocate_cell(
    (double)generate_random_c<double>().get_floating_point_range(
      min->to_double(),
      max->to_double()));
}