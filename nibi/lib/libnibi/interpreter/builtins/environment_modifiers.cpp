#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "interpreter/interpreter.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/keywords.hpp"
#include "macros.hpp"

#include <iterator>

namespace nibi {
namespace builtins {

cell_ptr builtin_fn_env_assignment(cell_processor_if &ci, cell_list_t &list,
                                   env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::ASSIGN, ==, 3)

  auto it = list.begin();
  std::advance(it, 1);

  if ((*it)->type != cell_type_e::SYMBOL) {
    throw interpreter_c::exception_c(
        "Expected symbol as first argument to assign", (*it)->locator);
  }

  auto &target_variable_name = (*it)->as_string();

  if (target_variable_name[0] == '$') {
    throw interpreter_c::exception_c(
        "Cannot assign to a variable starting with '$'", (*it)->locator);
  }

  auto target_assignment_value =
      ci.process_cell(ci.process_cell(list[2], env), env);

  // Explicitly clone the value as we might be reading from
  // an instruction that will be mutated later

  target_assignment_value = target_assignment_value->clone(env);

  env.set(target_variable_name, target_assignment_value);

  // Return a pointer to the new cell so assignments can be chained
  return target_assignment_value;
}

cell_ptr builtin_fn_env_set(cell_processor_if &ci, cell_list_t &list,
                            env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::SET, ==, 3)

  auto target_assignment_cell =
      ci.process_cell(ci.process_cell(list[1], env), env);

  auto target_assignment_value =
      ci.process_cell(ci.process_cell(list[2], env), env);

  // Then update that cell directly
  target_assignment_cell->update_from(*target_assignment_value, env);

  return target_assignment_cell;
}

cell_ptr builtin_fn_env_drop(cell_processor_if &ci, cell_list_t &list,
                             env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::DROP, >=, 2)

  for (auto it = std::next(list.begin()); it != list.end(); ++it) {
    if (!env.drop((*it)->as_symbol())) {
      throw interpreter_c::exception_c("Could not find symbol with name :" +
                                           (*it)->as_symbol(),
                                       (*it)->locator);
    }
  }
  return allocate_cell((int64_t)0);
}

cell_ptr assemble_anonymous_function(cell_processor_if &ci, cell_list_t &list,
                                     env_c &env) {
  auto it = list.begin();

  std::advance(it, 1);

  auto &function_argument_list = (*it)->as_list_info();

  if (function_argument_list.type != list_types_e::DATA) {
    throw interpreter_c::exception_c(
        "Expected data list `[]` for function arguments", (*it)->locator);
  }

  lambda_info_s lambda_info;

  lambda_info.arg_names.reserve(function_argument_list.list.size());

  for (auto &&arg : function_argument_list.list) {
    lambda_info.arg_names.push_back(arg->as_symbol());
  }

  std::advance(it, 1);
  lambda_info.body = (*it);
  if (lambda_info.body->type != cell_type_e::LIST) {
    throw interpreter_c::exception_c("Expected list for function body",
                                     lambda_info.body->locator);
  }

  function_info_s function_info("anon_fn", execute_suspected_lambda,
                                function_type_e::LAMBDA_FUNCTION, &env);

  function_info.lambda = {lambda_info};

  auto fn_cell = allocate_cell(function_info);

  return fn_cell;
}

cell_ptr builtin_fn_env_fn(cell_processor_if &ci, cell_list_t &list,
                           env_c &env) {
  if (list.size() == 3) {
    return assemble_anonymous_function(ci, list, env);
  }

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::FN, ==, 4)

  auto it = list.begin();

  std::advance(it, 1);

  auto target_function_name = (*it)->as_symbol();

  std::advance(it, 1);

  auto &function_argument_list = (*it)->as_list_info();

  if (function_argument_list.type != list_types_e::DATA) {
    throw interpreter_c::exception_c(
        "Expected data list `[]` for function arguments", (*it)->locator);
  }

  lambda_info_s lambda_info;

  lambda_info.arg_names.reserve(function_argument_list.list.size());

  for (auto &&arg : function_argument_list.list) {
    lambda_info.arg_names.push_back(arg->as_symbol());
  }

  std::advance(it, 1);
  lambda_info.body = (*it);
  if (lambda_info.body->type != cell_type_e::LIST) {
    throw interpreter_c::exception_c("Expected list for function body",
                                     lambda_info.body->locator);
  }

  function_info_s function_info(target_function_name, execute_suspected_lambda,
                                function_type_e::LAMBDA_FUNCTION, &env);

  function_info.lambda = {lambda_info};

  auto fn_cell = allocate_cell(function_info);

  // Set the variable
  env.set(target_function_name, fn_cell);

  return fn_cell;
}

cell_ptr handle_dict_access(cell_processor_if &ci, cell_list_t &list,
                            env_c &env) {

  auto definition = list[0];

  if (definition->type == cell_type_e::SYMBOL) {
    definition = env.get(definition->as_symbol());
  }

  auto fn_info = definition->as_function_info();
  auto dict = fn_info.operating_env->get("$data");

  // If its just the item then we will load and string the dict
  if (list.size() == 1) {
    return allocate_cell(dict->to_string(true, true));
  }

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::DICT, >=, 2)

  auto command = list[1]->as_symbol();

  auto &dict_value = dict->as_dict();

  if (command == ":keys") {
    list_info_s keys(list_types_e::DATA, {});
    for (auto &&dit : dict_value) {
      keys.list.push_back(allocate_cell(dit.first));
    }
    return allocate_cell(keys);
  }

  // Note: by not cloning the value we are allowing the user to
  // modify the value in the dict directly
  if (command == ":vals") {
    list_info_s vals(list_types_e::DATA, {});
    for (auto &&dit : dict_value) {
      vals.list.push_back(dit.second);
    }
    return allocate_cell(vals);
  }

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::DICT, >=, 3)

  auto key = ci.process_cell(ci.process_cell(list[2], env), env)->to_string();

  /*
      OPTIMIZATION


      TODO: A potential optimization here would be to have
      these commands stored in the function info's operating env
      or a separate env that is shared between all dict functions
      where these commands exist as symbols of cell_fn_t
      that tie into a lambda that performs the operation

  */

  if (command == ":let") {
    NIBI_LIST_ENFORCE_SIZE(nibi::kw::DICT, ==, 4)

    auto value = ci.process_cell(list[3], env);
    dict_value[key] = value;
    return dict_value[key];
  }

  if (command == ":get") {
    auto dit = dict_value.find(key);
    if (dit == dict_value.end()) {
      throw interpreter_c::exception_c(
          "Dict does not contain key `" + key + "`", list[2]->locator);
    }

    return dit->second;
  }

  if (command == ":del") {
    auto dit = dict_value.find(key);
    if (dit == dict_value.end()) {
      return allocate_cell((int64_t)0);
    }

    dict_value.erase(dit);
    return allocate_cell((int64_t)1);
  }

  throw interpreter_c::exception_c("Unknown dict command `" + command + "`",
                                   list[1]->locator);
}

cell_ptr builtin_fn_dict_fn(cell_processor_if &ci, cell_list_t &list,
                            env_c &env) {

  // If its size 1 then we make just an empty dict.
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::DICT, >=, 1)

  function_info_s function_info("dict", handle_dict_access,
                                function_type_e::FAUX, new env_c());

  cell_dict_t dict_actual;

  // No-value dict
  if (list.size() == 1) {

    auto cell_actual = allocate_cell(dict_actual);
    cell_actual->locator = list[0]->locator;
    function_info.operating_env->set_new_alloc("$data", cell_actual);

    auto fn_actual = allocate_cell(function_info);
    return fn_actual;
  }

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::DICT, ==, 2)

  // If the next item is a symbol we should resolve it
  // because it may hold a list of key value pairs.
  cell_ptr dict_value{nullptr};
  if (list[1]->type == cell_type_e::SYMBOL) {
    dict_value = ci.process_cell(list[1], env);
  } else {
    dict_value = list[1];
  }

  auto &list_info = dict_value->as_list_info();

  if (list_info.type != list_types_e::DATA) {
    throw interpreter_c::exception_c("Expected data list `[]` for dict values",
                                     list[1]->locator);
  }

  if (list_info.list.size() != 0) {
    // Walk the list and ensure that each item is a list of size 2

    for (auto &value : list_info.list) {
      auto resolved_value = ci.process_cell(value, env);
      auto &resolved_list_info = resolved_value->as_list_info();
      if (resolved_list_info.type != list_types_e::DATA) {
        throw interpreter_c::exception_c(
            "Expected data list `[]` for dict values", value->locator);
      }
      if (resolved_list_info.list.size() != 2) {
        throw interpreter_c::exception_c(
            "Expected list of size 2 for dict values [key value]",
            value->locator);
      }
      if (resolved_list_info.list[0]->type != cell_type_e::STRING) {
        throw interpreter_c::exception_c("Expected string for dict key",
                                         resolved_list_info.list[0]->locator);
      }

      dict_actual[resolved_list_info.list[0]->to_string()] =
          ci.process_cell(resolved_list_info.list[1], env);
    }
  }

  auto cell_actual = allocate_cell(dict_actual);
  cell_actual->locator = list[0]->locator;

  function_info.operating_env->set_new_alloc("$data", cell_actual);

  auto fn_actual = allocate_cell(function_info);
  fn_actual->locator = list[0]->locator;
  return fn_actual;
}

} // namespace builtins
} // namespace nibi
