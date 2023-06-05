
#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "interpreter/interpreter.hpp"
#include "libnibi/cell.hpp"
#include "macros.hpp"

namespace nibi {

namespace builtins {

#define NIBI_CONVERSION_TO_TYPE(type, conversion_method)                       \
  auto value = list_get_nth_arg(ci, 1, list, env);                             \
  try {                                                                        \
    int64_t result = std::stoll(value->to_string());                           \
    return allocate_cell(result);                                              \
  } catch (std::invalid_argument & e) {                                        \
    throw interpreter_c::exception_c(                                          \
        std::string("Invalid argument to integer conversion: ") +              \
            value->to_string(),                                                \
        value->locator);                                                       \
  } catch (std::out_of_range & e) {                                            \
    throw interpreter_c::exception_c(                                          \
        std::string("Out of range argument to integer conversion: ") +         \
            value->to_string(),                                                \
        value->locator);                                                       \
  } catch (...) {                                                              \
    throw interpreter_c::exception_c(                                          \
        std::string("Unknown error in integer conversion: ") +                 \
            value->to_string(),                                                \
        value->locator);                                                       \
  }                                                                            \
  return allocate_cell((type)0);

cell_ptr builtin_fn_cvt_to_string(interpreter_c &ci, cell_list_t &list,
                                  env_c &env) {

  NIBI_LIST_ENFORCE_SIZE("str", ==, 2)
  auto value = list_get_nth_arg(ci, 1, list, env);
  return allocate_cell(value->to_string());
}

cell_ptr builtin_fn_cvt_to_integer(interpreter_c &ci, cell_list_t &list,
                                   env_c &env){
    NIBI_LIST_ENFORCE_SIZE("int", ==, 2)
        NIBI_CONVERSION_TO_TYPE(int64_t, std::stoll)}

cell_ptr
    builtin_fn_cvt_to_float(interpreter_c &ci, cell_list_t &list, env_c &env){
        NIBI_LIST_ENFORCE_SIZE("float", ==, 2)
            NIBI_CONVERSION_TO_TYPE(double, std::stod)}

cell_ptr
    builtin_fn_cvt_to_split(interpreter_c &ci, cell_list_t &list, env_c &env) {
  NIBI_LIST_ENFORCE_SIZE("split", >=, 2)
  auto value = list_get_nth_arg(ci, 1, list, env);

  if (value->type == cell_type_e::LIST) {
    NIBI_LIST_ENFORCE_SIZE("split (with list)", ==, 3)
    auto as_list = value->to_list();

    int64_t count = 0;
    auto target = list_get_nth_arg(ci, 2, list, env)->to_integer();

    if (0 == target) {
      return value->clone(env);
    }
    cell_list_t data_list;
    cell_list_t inner_list;

    for (auto &cell : as_list) {
      if (count == target) {
        data_list.push_back(
            allocate_cell(list_info_s{list_types_e::DATA, inner_list}));
        inner_list.clear();
        count = 0;
      }
      inner_list.push_back(cell->clone(env));
      count++;
    }
    if (inner_list.size()) {
      data_list.push_back(
          allocate_cell(list_info_s{list_types_e::DATA, inner_list}));
    }
    return allocate_cell(list_info_s{list_types_e::DATA, data_list});
  }
  NIBI_LIST_ENFORCE_SIZE("split", ==, 2)

  auto as_string = value->to_string();
  cell_list_t data_list;

  for (auto ch : as_string) {
    data_list.push_back(allocate_cell(std::string(1, ch)));
  }

  return allocate_cell(list_info_s{list_types_e::DATA, data_list});
}

} // namespace builtins

} // namespace nibi
