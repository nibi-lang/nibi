#include "cell.hpp"

namespace {
  const char *function_type_to_string(function_type_e type) {
    switch(type) {
      case function_type_e::UNSET:
        return "UNSET";
      case function_type_e::BUILTIN_CPP_FUNCTION:
        return "BUILTIN_CPP_FUNCTION";
      case function_type_e::EXTERNAL_FUNCTION:
        return "EXTERNAL_FUNCTION";
      case function_type_e::LAMBDA_FUNCTION:
        return "LAMBDA_FUNCTION";
    }
    return "UNKNOWN";
  }
}

int64_t cell_c::to_integer() {
  return this->as_integer();
}

int64_t& cell_c::as_integer() {
  try
  {
    return std::any_cast<int64_t&>(this->data);
  }
  catch (const std::bad_any_cast& e)
  {
      throw cell_access_exception_c(
        "Cell is not an integer", this->locator);
  }
}

double cell_c::to_double() {
  return this->as_double();
}

double& cell_c::as_double() {
  try
  {
    return std::any_cast<double&>(this->data);
  }
  catch (const std::bad_any_cast& e)
  {
      throw cell_access_exception_c(
        "Cell is not a double", this->locator);
  }
}

cell_list_t cell_c::to_list() {
  return this->as_list();
}

cell_list_t& cell_c::as_list() {
    try
    {
      return std::any_cast<cell_list_t&>(this->data);
    }
    catch (const std::bad_any_cast& e)
    {
        throw cell_access_exception_c(
          "Cell is not a list", this->locator);
    }
}

cell_c* cell_c::as_reference() {
  try
  {
    return std::any_cast<cell_c*>(this->data);
  }
  catch(const std::exception& e)
  {
        throw cell_access_exception_c(
          "Cell is not a reference to another cell", this->locator);
  }
}

aberrant_cell_if* cell_c::as_aberrant() {
  try 
  {
    return std::any_cast<aberrant_cell_if*>(this->data);
  }
  catch (const std::bad_any_cast& e)
  {
      throw cell_access_exception_c(
        "Cell is not an aberrant cell", this->locator);
  }
}

function_info_s& cell_c::as_function() {
  try
  {
    return std::any_cast<function_info_s&>(this->data);
  }
  catch (const std::bad_any_cast& e)
  {
      throw cell_access_exception_c(
        "Cell is not a function", this->locator);
  }
}

std::string cell_c::to_string() {
  switch (this->type)
  {
    case cell_type_e::NIL:
      return "nil";
    case cell_type_e::INTEGER:
      return std::to_string(this->as_integer());
    case cell_type_e::DOUBLE:
      return std::to_string(this->as_double());
    case cell_type_e::STRING:
      return this->as_string();
    case cell_type_e::REFERENCE: {
      auto* cell = this->as_reference();
      if (!cell)
        return "nil";
      else
        return cell->to_string();
      return cell->to_string();
    }
    case cell_type_e::ABERRANT: {
      aberrant_cell_if* cell = this->as_aberrant();
      if (!cell)
        return "nil";
      else
        return cell->represent_as_string();
    }
    case cell_type_e::FUNCTION: {
      auto fn = this->as_function();
      std::string result = "<function:";
      result += fn.name;
      result += ", type:";
      result += function_type_to_string(fn.type);
      result += ">";
      return result;
    }
    case cell_type_e::LIST: {
      std::string result = "(";
      auto& list = this->as_list();
      for (auto* cell : list) {
        result += cell->to_string() + " ";
      }
      result.pop_back();
      result += ")";
      return result;
    }
  }
  throw cell_access_exception_c(
    "Unknown cell type", this->locator);
}

std::string& cell_c::as_string() {
  try
  {
    return std::any_cast<std::string&>(this->data);
  }
  catch (const std::bad_any_cast& e)
  {
      throw cell_access_exception_c(
        "Cell is not a string", this->locator);
  }
}