#include "libnibi/cell.hpp"

#include "libnibi/environment.hpp"

#include <iostream>

#if NIBI_USE_CELL_POOL
#include "libnibi/cell_pool.hpp"
#endif

namespace nibi {

#if NIBI_USE_CELL_POOL
void deallocate_cell_to_pool(cell_c *cell) {
  cell_pool_c::instance().deallocate(cell);
}
#endif

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
  case function_type_e::FAUX:
    return "FAUX";
  }
  return "UNKNOWN";
}
} // namespace

const char *cell_type_to_string(const cell_type_e type) {
  switch (type) {
  case cell_type_e::NIL:
    return "NIL";
  case cell_type_e::I8:
    return "I8";
  case cell_type_e::I16:
    return "I16";
  case cell_type_e::I32:
    return "I32";
  case cell_type_e::I64:
    return "I64";
  case cell_type_e::U8:
    return "U8";
  case cell_type_e::U16:
    return "U16";
  case cell_type_e::U32:
    return "U32";
  case cell_type_e::U64:
    return "U64";
  case cell_type_e::F32:
    return "F32";
  case cell_type_e::F64:
    return "F64";
  case cell_type_e::PTR:
    return "PTR";
  case cell_type_e::CHAR:
    return "CHAR";
  case cell_type_e::STRING:
    return "STRING";
  case cell_type_e::FUNCTION:
    return "FUNCTION";
  case cell_type_e::SYMBOL:
    return "SYMBOL";
  case cell_type_e::LIST:
    return "LIST";
  case cell_type_e::DICT:
    return "DICT";
  case cell_type_e::ALIAS:
    return "ALIAS";
  case cell_type_e::ABERRANT:
    return "ABERRANT";
  case cell_type_e::ENVIRONMENT:
    return "ENVIRONMENT";
  }
  return "UNKNOWN";
}

cell_c::~cell_c() {
  // Different types of cells may need to be manually cleaned up
  switch (this->type) {
  case cell_type_e::ABERRANT: {
    if (this->data.aberrant != nullptr) {
      delete this->data.aberrant;
      this->data.aberrant = nullptr;
    }
    break;
  }
  // Check if the function is faux, if so,
  // we need to clean the operating environment
  // as well because fauxs own their own
  case cell_type_e::FUNCTION: {
    auto &func_info = this->as_function_info();
    if (func_info.type == function_type_e::FAUX) {
      if (func_info.operating_env) {
        delete func_info.operating_env;
        func_info.operating_env = nullptr;
      }
    }
    delete this->data.fn;
    this->data.fn = nullptr;
    break;
  }
  case cell_type_e::SYMBOL: {
    if (this->data.sym) {
      delete this->data.sym;
      this->data.sym = nullptr;
    }
    break;
  }
  case cell_type_e::STRING: {
    if (this->data.cstr) {
      delete[] this->data.cstr;
      this->data.cstr = nullptr;
    }
    break;
  }
  case cell_type_e::ENVIRONMENT: {
    if (this->data.env) {
      delete this->data.env;
      this->data.env = nullptr;
    }
  }
  case cell_type_e::ALIAS: {
    if (this->data.alias) {
      delete this->data.alias;
      this->data.alias = nullptr;
    }
    break;
  }
  case cell_type_e::DICT: {
    if (this->data.dict) {
      delete this->data.dict;
      this->data.dict = nullptr;
    }
    break;
  }
  case cell_type_e::LIST: {
    if (this->data.list) {
      delete this->data.list;
      this->data.list = nullptr;
    }
    break;
  }
  default: {
    break;
  }
  }
}

cell_ptr cell_c::clone(env_c &env, bool resolve_sym) {

  // Allocate a new cell
  cell_ptr new_cell = allocate_cell(this->type);

  // Copy the data
  if (this->locator) {
    new_cell->locator = this->locator->clone();
  } else {
    new_cell->locator = nullptr;
  }

  switch (this->type) {
  case cell_type_e::NIL:
    return allocate_cell(cell_type_e::NIL);

  case cell_type_e::I8:
    new_cell->data.i8 = this->data.i8;
    break;
  case cell_type_e::I16:
    new_cell->data.i16 = this->data.i16;
    break;
  case cell_type_e::I32:
    new_cell->data.i32 = this->data.i32;
    break;
  case cell_type_e::I64:
    new_cell->data.i64 = this->data.i64;
    break;
  case cell_type_e::U8:
    new_cell->data.u8 = this->data.u8;
    break;
  case cell_type_e::U16:
    new_cell->data.u16 = this->data.u16;
    break;
  case cell_type_e::U32:
    new_cell->data.u32 = this->data.u32;
    break;
  case cell_type_e::U64:
    new_cell->data.u64 = this->data.u64;
    break;
  case cell_type_e::F32:
    new_cell->data.f32 = this->data.f32;
    break;
  case cell_type_e::F64:
    new_cell->data.f64 = this->data.f64;
    break;
  case cell_type_e::CHAR:
    new_cell->data.ch = this->data.ch;
    break;
  case cell_type_e::PTR: {
    new_cell->data.ptr = this->data.ptr;
    break;
  }
  case cell_type_e::ALIAS: {
    new_cell = this->get_alias()->clone(env);
    break;
  }
  case cell_type_e::SYMBOL: {
    auto referenced_symbol = env.get(this->as_symbol());
    if (resolve_sym && referenced_symbol != nullptr) {
      new_cell = referenced_symbol->clone(env, resolve_sym);
    } else {
      new_cell->data.sym->value = this->as_symbol();
    }
    break;
  }
  case cell_type_e::STRING:
    new_cell->update_string(this->to_string());
    break;
  case cell_type_e::FUNCTION: {

    auto &func_info = this->as_function_info();

    new_cell->data.fn->name = func_info.name;
    new_cell->data.fn->fn = func_info.fn;
    new_cell->data.fn->type = func_info.type;
    new_cell->data.fn->isolate = func_info.isolate;

    if (func_info.type == function_type_e::FAUX && func_info.operating_env) {
      new_cell->as_function_info().operating_env = new env_c();
      *new_cell->as_function_info().operating_env = *func_info.operating_env;

    } else if (func_info.operating_env) {
      // Other functions may be pointing to an env that they don't own
      // so we need to set o set the pointer
      new_cell->as_function_info().operating_env = func_info.operating_env;
    }

    // Copy lambda stuff over
    if (func_info.lambda.has_value()) {
      new_cell->as_function_info().lambda = func_info.lambda;
      // When copying instructions we need to ensure we don't resolve
      // symbols at any depth so we don't accidentally kick anything
      // off.
      (*new_cell->as_function_info().lambda).body =
          (*func_info.lambda).body->clone(env, false);
    }
    break;
  }
  case cell_type_e::LIST: {
    auto &linf = this->as_list_info();
    auto &other = new_cell->as_list_info();
    other.type = linf.type;
    for (auto &cell : linf.list) {
      other.list.push_back(cell->clone(env, resolve_sym));
    }
    break;
  }
  case cell_type_e::ENVIRONMENT: {
    throw cell_access_exception_c("Cannot clone an environment", this->locator);
    break;
  }
  case cell_type_e::DICT: {
    auto &dinf = this->as_dict();
    auto &other = new_cell->as_dict();
    for (auto &pair : dinf) {
      other[pair.first] = pair.second->clone(env, resolve_sym);
    }
    break;
  }
  case cell_type_e::ABERRANT: {
    auto cloned = this->as_aberrant()->clone();
    if (!cloned) {
      throw cell_access_exception_c(
          std::string("Cannot clone given aberrant: ") +
              this->data.aberrant->represent_as_string(),
          this->locator);
    }
    new_cell->data.aberrant = cloned;
    break;
  }
  }
  return new_cell;
}

std::string cell_c::to_string(bool quote_strings, bool flatten_complex) {
  switch (this->type) {
  case cell_type_e::NIL:
    return "nil";
  case cell_type_e::I8:
    return std::to_string(this->data.i8);
  case cell_type_e::I16:
    return std::to_string(this->data.i16);
  case cell_type_e::I32:
    return std::to_string(this->data.i32);
  case cell_type_e::I64:
    return std::to_string(this->data.i64);
  case cell_type_e::U8:
    return std::to_string(this->data.u8);
  case cell_type_e::U16:
    return std::to_string(this->data.u16);
  case cell_type_e::U32:
    return std::to_string(this->data.u32);
  case cell_type_e::U64:
    return std::to_string(this->data.u64);
  case cell_type_e::F32:
    return std::to_string(this->data.f32);
  case cell_type_e::F64:
    return std::to_string(this->data.f64);
  case cell_type_e::CHAR:
    if (quote_strings) {
      return "'" + std::string(1, this->data.ch) + "'";
    }
    return std::string(1, this->data.ch);
  case cell_type_e::PTR:
    return std::to_string((uint64_t)this->data.ptr);
  case cell_type_e::SYMBOL:
    return this->as_symbol();
  case cell_type_e::STRING:
    if (quote_strings) {
      return "\"" + this->as_string() + "\"";
    }
    return this->as_string();
  case cell_type_e::ABERRANT: {
    aberrant_cell_if *cell = this->as_aberrant();
    if (!cell)
      return "nil";
    else {
      // We prefix the string so external mods
      // cant inject code into the interpreter
      // by returning a string that can be evaluated
      // using eval
      return "Aberrant cell: " + cell->represent_as_string();
    }
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
  case cell_type_e::DICT: {
    auto &dict = this->as_dict();
    std::string result = "{";
    for (auto &pair : dict) {
      result += pair.first + ":" + pair.second->to_string(quote_strings) + " ";
    }
    if (result.size() > 1)
      result.pop_back();
    result += "}";
    return result;
  }
  case cell_type_e::ALIAS: {
    auto alias = this->get_alias();
    return alias->to_string(quote_strings, flatten_complex);
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

} // namespace nibi
