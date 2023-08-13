#include <iostream>

#include "interpreter/builtins/builtins.hpp"
#include "interpreter/interpreter.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/keywords.hpp"
#include "macros.hpp"

#include <iterator>

namespace nibi {
namespace builtins {

cell_ptr builtin_fn_env_alias(interpreter_c &ci, cell_list_t &list,
                              env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::ALIAS, ==, 3)

  auto alias_target = ci.process_cell(list[1], env);
  auto target_variable_name = list[2]->as_c_string();

  NIBI_VALIDATE_VAR_NAME(target_variable_name, list[2]->locator);

  if (list[1]->type == cell_type_e::SYMBOL) {
    auto source_variable_name = list[1]->as_c_string();
    if (::strcmp(source_variable_name, target_variable_name) == 0) {
      throw interpreter_c::exception_c("Cannot alias a variable to itself",
                                       list[1]->locator);
    }
  }

  auto cell = allocate_cell(alias_s{alias_target});
  cell->locator = alias_target->locator;

  env.set(target_variable_name, cell);

  return allocate_cell(cell_type_e::NIL);
}

cell_ptr builtin_fn_env_str_set_at(interpreter_c &ci, cell_list_t &list,
                                   env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::STR_SET_AT, ==, 4)

  auto target_cell = ci.process_cell(list[1], env);
  auto target = target_cell->as_string();

  auto index = ci.process_cell(list[2], env)->as_integer();
  auto value = ci.process_cell(list[3], env)->to_string();

  while (index < 0) {
    index = target.size() + index;
  }

  if (index >= target.size()) {
    throw interpreter_c::exception_c("Index out of bounds", list[2]->locator);
  }

  std::string result =
      target.substr(0, index) + value + target.substr(index + 1);

  target_cell->update_from(*allocate_cell(result), env);

  return target_cell;
}

cell_ptr builtin_fn_env_assignment(interpreter_c &ci, cell_list_t &list,
                                   env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::ASSIGN, ==, 3)

  auto it = list.begin();
  std::advance(it, 1);

  if ((*it)->type != cell_type_e::SYMBOL) {
    throw interpreter_c::exception_c(
        "Expected symbol as first argument to assign", (*it)->locator);
  }

  auto target_variable_name = (*it)->as_c_string();

  NIBI_VALIDATE_VAR_NAME(target_variable_name, (*it)->locator);

  auto target_assignment_value = ci.process_cell(list[2], env);
  // ci.process_cell(ci.process_cell(list[2], env), env);

  // Explicitly clone the value as we might be reading from
  // an instruction that will be mutated later

  target_assignment_value = target_assignment_value->clone(env);

  env.set(target_variable_name, target_assignment_value);

  // Return a pointer to the new cell so assignments can be chained
  return target_assignment_value;
}

cell_ptr builtin_fn_env_set(interpreter_c &ci, cell_list_t &list, env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::SET, ==, 3)

  auto target_assignment_cell = ci.process_cell(list[1], env);
  // ci.process_cell(ci.process_cell(list[1], env), env);

  auto target_assignment_value = ci.process_cell(list[2], env);
  // ci.process_cell(ci.process_cell(list[2], env), env);

  // Then update that cell directly
  target_assignment_cell->update_from(*target_assignment_value, env);

  return target_assignment_cell;
}

cell_ptr builtin_fn_env_drop(interpreter_c &ci, cell_list_t &list, env_c &env) {
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

cell_ptr assemble_anonymous_function(interpreter_c &ci, cell_list_t &list,
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
  fn_cell->locator = list[0]->locator;

  return std::move(fn_cell);
}

cell_ptr builtin_fn_env_fn(interpreter_c &ci, cell_list_t &list, env_c &env) {
  if (list.size() == 3) {
    return std::move(assemble_anonymous_function(ci, list, env));
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
  fn_cell->locator = list[0]->locator;

  // Set the variable
  env.set(target_function_name, fn_cell);

  return std::move(fn_cell);
}

cell_ptr handle_dict_access(interpreter_c &ci, cell_list_t &list, env_c &env) {

  auto definition = list[0];

  if (definition->type == cell_type_e::SYMBOL) {
    definition = env.get(definition->as_symbol());
  }

  auto fn_info = definition->as_function_info();
  auto dict = fn_info.operating_env->get("$data");

  // If its just the item then we will load and string the dict
  if (list.size() == 1) {
    auto c = allocate_cell(dict->to_string(true, true));
    c->locator = list[0]->locator;
    return std::move(c);
  }

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::DICT, >=, 2)

  auto command = list[1]->as_symbol();

  auto &dict_value = dict->as_dict();

  if (command == ":keys") {
    list_info_s keys(list_types_e::DATA);
    for (auto &&dit : dict_value) {
      keys.list.push_back(allocate_cell(dit.first));
    }
    auto c = allocate_cell(keys);
    c->locator = list[1]->locator;
    return std::move(c);
  }

  // Note: by not cloning the value we are allowing the user to
  // modify the value in the dict directly
  if (command == ":vals") {
    list_info_s vals(list_types_e::DATA);
    for (auto &&dit : dict_value) {
      vals.list.push_back(dit.second);
    }
    auto c = allocate_cell(vals);
    c->locator = list[1]->locator;
    return std::move(c);
  }

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::DICT, >=, 3)

  auto key = ci.process_cell(list[2], env)->to_string();
  // auto key = ci.process_cell(ci.process_cell(list[2], env),
  // env)->to_string();

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
    value->locator = list[3]->locator;
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

cell_ptr builtin_fn_dict_fn(interpreter_c &ci, cell_list_t &list, env_c &env) {

  // If its size 1 then we make just an empty dict.
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::DICT, >=, 1)

  function_info_s function_info("dict", handle_dict_access,
                                function_type_e::FAUX, new env_c());

  cell_dict_t dict_actual;

  // No-value dict
  if (list.size() == 1) {

    auto cell_actual = allocate_cell(dict_actual);
    cell_actual->locator = list[0]->locator;
    function_info.operating_env->set("$data", cell_actual);
    function_info.operating_env->set("$is_dict", allocate_cell((int64_t)1));
    auto fn_actual = allocate_cell(function_info);
    fn_actual->locator = list[0]->locator;
    return std::move(fn_actual);
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

  function_info.operating_env->set("$data", cell_actual);
  function_info.operating_env->set("$is_dict", allocate_cell((int64_t)1));

  auto fn_actual = allocate_cell(function_info);
  fn_actual->locator = list[0]->locator;
  return std::move(fn_actual);
}

} // namespace builtins
} // namespace nibi
