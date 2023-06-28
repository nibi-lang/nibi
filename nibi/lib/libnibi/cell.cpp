#include "libnibi/cell.hpp"

#include "libnibi/environment.hpp"

#include <iostream>

namespace nibi {
namespace {
const char *function_type_to_string(function_type_e type) {
  switch (type) {
  case function_type_e::UNSET:
    return "UNSET";
  case function_type_e::BUILTIN_CPP_FUNCTION:
    return "BUILTIN_CPP_FUNCTION";
  case function_type_e::EXTERNAL_FUNCTION:
    return "EXTERNAL_FUNCTION";
  case function_type_e::LAMBDA_FUNCTION:
    return "LAMBDA_FUNCTION";
  case function_type_e::MACRO:
    return "MACRO";
  }
  return "UNKNOWN";
}
} // namespace

const char *cell_type_to_string(const cell_type_e type) {
  switch (type) {
  case cell_type_e::NIL:
    return "NIL";
  case cell_type_e::INTEGER:
    return "INTEGER";
  case cell_type_e::DOUBLE:
    return "DOUBLE";
  case cell_type_e::STRING:
    return "STRING";
  case cell_type_e::FUNCTION:
    return "FUNCTION";
  case cell_type_e::SYMBOL:
    return "SYMBOL";
  case cell_type_e::LIST:
    return "LIST";
  }
  return "UNKNOWN";
}

cell_c::~cell_c() {
  // Different types of cells may need to be manually cleaned up
  switch (this->type) {
  case cell_type_e::ENVIRONMENT: {
    auto &env_info = this->as_environment_info();
    if (env_info.env != nullptr) {
      delete env_info.env;
    }
    break;
  }
  case cell_type_e::ABERRANT: {
    auto *aberrant = this->as_aberrant();
    if (aberrant != nullptr) {
      delete aberrant;
    }
    break;
  }
  // Check if the function is a macro, if so,
  // we need to clean the operating environment
  // as well because macros own their own
  case cell_type_e::FUNCTION: {
    auto &func_info = this->as_function_info();
    if (func_info.type == function_type_e::MACRO) {
      if (func_info.operating_env) {
        delete func_info.operating_env;
      }
    }
    break;
  }
  default: {
    break;
  }
  }
}

cell_ptr cell_c::clone(env_c &env) {

  // Allocate a new cell
  cell_ptr new_cell = allocate_cell(this->type);

  // Copy the data
  new_cell->locator = this->locator;

  switch (this->type) {
  case cell_type_e::NIL:
    return allocate_cell(cell_type_e::NIL);
  case cell_type_e::INTEGER:
    new_cell->data = this->to_integer();
    break;
  case cell_type_e::DOUBLE:
    new_cell->data = this->to_double();
    break;
  case cell_type_e::SYMBOL: {
    auto referenced_symbol = env.get(this->as_symbol());
    if (referenced_symbol == nullptr) {
      throw cell_access_exception_c("Unknown variable", this->locator);
    }
    new_cell = referenced_symbol->clone(env);
    break;
  }
  case cell_type_e::STRING:
    new_cell->data = this->to_string();
    break;
  case cell_type_e::FUNCTION: {

    // Note -> This will store a reference to the function info
    //        which is fine because the function info is stored
    auto &func_info = this->as_function_info();

    new_cell->data =
        function_info_s(func_info.name, func_info.fn, func_info.type);

    if (func_info.type == function_type_e::MACRO) {
      if (func_info.operating_env) {
        new_cell->as_function_info().operating_env = new env_c();
        *new_cell->as_function_info().operating_env = *func_info.operating_env;
      }
    }
    break;
  }
  case cell_type_e::LIST: {
    auto &linf = this->as_list_info();
    auto &other = new_cell->as_list_info();
    other.type = linf.type;
    for (auto &cell : linf.list) {
      other.list.push_back(cell->clone(env));
    }
    break;
  }
  case cell_type_e::ENVIRONMENT: {
    throw cell_access_exception_c("Cannot clone an environment", this->locator);
    break;
  }
  case cell_type_e::ABERRANT: {
    new_cell->data = this->as_aberrant();
    break;
  }
  }
  return new_cell;
}

void cell_c::update_from(cell_c &other, env_c &env) {
  this->type = other.type;
  this->data = other.clone(env)->data;
}

int64_t cell_c::to_integer() {
  if (this->type == cell_type_e::DOUBLE) {
    return (int64_t)this->as_double();
  }
  return this->as_integer();
}

int64_t &cell_c::as_integer() {
  try {
    return std::any_cast<int64_t &>(this->data);
  } catch (const std::bad_any_cast &e) {
    throw cell_access_exception_c(
        "Cell is not an integer: " + this->to_string(), this->locator);
  }
}

double cell_c::to_double() {
  if (this->type == cell_type_e::INTEGER) {
    return (double)this->as_integer();
  }
  return this->as_double();
}

double &cell_c::as_double() {
  try {
    return std::any_cast<double &>(this->data);
  } catch (const std::bad_any_cast &e) {
    throw cell_access_exception_c("Cell is not a double", this->locator);
  }
}

cell_list_t cell_c::to_list() { return this->as_list(); }

cell_list_t &cell_c::as_list() {
  try {
    auto &info = as_list_info();
    return info.list;
  } catch (const std::bad_any_cast &e) {
    throw cell_access_exception_c("Cell is not a list", this->locator);
  }
}

list_info_s cell_c::to_list_info() { return this->as_list_info(); }

list_info_s &cell_c::as_list_info() {
  try {
    return std::any_cast<list_info_s &>(this->data);
  } catch (const std::bad_any_cast &e) {
    throw cell_access_exception_c("Cell is not a list", this->locator);
  }
}

aberrant_cell_if *cell_c::as_aberrant() {
  try {
    return std::any_cast<aberrant_cell_if *>(this->data);
  } catch (const std::bad_any_cast &e) {
    throw cell_access_exception_c("Cell is not an aberrant cell",
                                  this->locator);
  }
}

function_info_s &cell_c::as_function_info() {
  try {
    return std::any_cast<function_info_s &>(this->data);
  } catch (const std::bad_any_cast &e) {
    throw cell_access_exception_c("Cell is not a function", this->locator);
  }
}

environment_info_s &cell_c::as_environment_info() {
  try {
    return std::any_cast<environment_info_s &>(this->data);
  } catch (const std::bad_any_cast &e) {
    throw cell_access_exception_c("Cell is not an environment", this->locator);
  }
}

std::string cell_c::to_string(bool quote_strings, bool flatten_complex) {
  switch (this->type) {
  case cell_type_e::NIL:
    return "nil";
  case cell_type_e::INTEGER:
    return std::to_string(this->as_integer());
  case cell_type_e::DOUBLE:
    return std::to_string(this->as_double());
  case cell_type_e::SYMBOL:
    return this->as_string();
  case cell_type_e::STRING:
    if (quote_strings) {
      return "\"" + this->as_string() + "\"";
    }
    return this->as_string();
  case cell_type_e::ABERRANT: {
    aberrant_cell_if *cell = this->as_aberrant();
    if (!cell)
      return "nil";
    else
      return cell->represent_as_string();
  }
  case cell_type_e::FUNCTION: {
    auto &fn = this->as_function_info();
    if (flatten_complex) {
      return fn.name;
    }
    std::string result = "<function:";
    result += fn.name;
    result += ", type:";
    result += function_type_to_string(fn.type);
    result += ">";
    return result;
  }
  case cell_type_e::ENVIRONMENT: {
    auto &env = this->as_environment_info();
    if (flatten_complex) {
      return env.name;
    }
    std::string result = "<environment:";
    result += env.name;
    result += ">";
    return result;
  }
  case cell_type_e::LIST: {
    std::string result;
    auto &list_info = this->as_list_info();

    switch (list_info.type) {
    case list_types_e::INSTRUCTION: {
      result += "(";
      for (auto cell : list_info.list) {
        result += cell->to_string(quote_strings, flatten_complex) + " ";
      }
      if (result.size() > 1)
        result.pop_back();
      result += ")";
      break;
    }
    case list_types_e::DATA: {
      result += "[";
      for (auto cell : list_info.list) {
        result += cell->to_string(quote_strings, flatten_complex) + " ";
      }
      if (result.size() > 1)
        result.pop_back();
      result += "]";
      break;
    }
    case list_types_e::ACCESS: {
      result += "{";
      for (auto cell : list_info.list) {
        result += cell->to_string(quote_strings, flatten_complex) + " ";
      }
      if (result.size() > 1)
        result.pop_back();
      result += "}";
      break;
    }
    }
    return result;
  }
  }
  throw cell_access_exception_c("Unknown cell type", this->locator);
}

std::string &cell_c::as_string() {
  try {
    return std::any_cast<std::string &>(this->data);
  } catch (const std::bad_any_cast &e) {
    throw cell_access_exception_c("Cell is not a string", this->locator);
  }
}

std::string &cell_c::as_symbol() {
  if (this->type != cell_type_e::SYMBOL) {
    throw cell_access_exception_c("Cell is not a symbol", this->locator);
  }
  try {
    return std::any_cast<std::string &>(this->data);
  } catch (const std::bad_any_cast &e) {
    throw cell_access_exception_c("Symbol cell does not contain a string",
                                  this->locator);
  }
}
} // namespace nibi
