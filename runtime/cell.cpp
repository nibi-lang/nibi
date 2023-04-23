#include "cell.hpp"
#include "runtime/runtime.hpp"
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
  }
  return "UNKNOWN";
}
} // namespace

cell_c *global_cell_nil{nullptr};
cell_c *global_cell_true{nullptr};
cell_c *global_cell_false{nullptr};

void global_cells_destroy() {
  if (global_cell_nil) {
    delete global_cell_nil;
    global_cell_nil = nullptr;
  }

  if (global_cell_true) {
    delete global_cell_true;
    global_cell_true = nullptr;
  }

  if (global_cell_false) {
    delete global_cell_false;
    global_cell_false = nullptr;
  }
}

bool global_cells_initialize() {
  global_cell_nil = new cell_c(cell_type_e::NIL);
  global_cell_true = new cell_c((int64_t)1);
  global_cell_false = new cell_c((int64_t)0);

  if (global_cell_true && global_cell_false && global_cell_nil) {
    return true;
  }

  // If we get here, something went wrong so we need to clean up
  global_cells_destroy();
  return false;
}

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
  case cell_type_e::LIST:
    return "LIST";
  case cell_type_e::REFERENCE:
    return "REFERENCE";
  }
  return "UNKNOWN";
}

cell_c::~cell_c() {
  if (this->type == cell_type_e::LIST) {
    auto &info = this->as_list_info();
    for (auto &cell : info.list) {
      /*
        By marking this as not in use it will be 
        recycled by the memory manager on the next
        pass. This means that deletions will
        be performed in a batched manner.
      */
      cell->mark_as_in_use(false);
    }
  }
}

cell_c* cell_c::clone() {

  // Allocate a new cell
  cell_c* new_cell = global_runtime->get_runtime_memory().allocate(this->type);

  // Copy the data
  new_cell->locator = this->locator;

  switch (this->type) {
  case cell_type_e::NIL:
    new_cell->mark_as_in_use(false);
    return global_cell_nil;
  case cell_type_e::INTEGER:
    new_cell->data = this->as_integer();
    break;
  case cell_type_e::DOUBLE:
    new_cell->data = this->as_double();
    break;
  case cell_type_e::STRING:
    new_cell->data = this->as_string();
    break;
  case cell_type_e::FUNCTION:
    new_cell->data = this->as_function_info();
    break;
  case cell_type_e::REFERENCE:
    new_cell->data = this->to_referenced_cell();
    break;
  case cell_type_e::LIST:
    list_info_s linf = this->as_list_info();
    list_info_s other = new_cell->as_list_info();
    for(auto& cell : linf.list) {
      other.list.push_back(cell->clone());
    }
    break;
  }

  return new_cell;
}

int64_t cell_c::to_integer() { return this->as_integer(); }

int64_t &cell_c::as_integer() {
  try {
    return std::any_cast<int64_t &>(this->data);
  } catch (const std::bad_any_cast &e) {
    throw cell_access_exception_c("Cell is not an integer", this->locator);
  }
}

double cell_c::to_double() { return this->as_double(); }

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

cell_c *cell_c::to_referenced_cell() {
  try {
    return std::any_cast<cell_c *>(this->data);
  } catch (const std::exception &e) {
    throw cell_access_exception_c("Cell is not a reference to another cell",
                                  this->locator);
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

std::string cell_c::to_string() {
  switch (this->type) {
  case cell_type_e::NIL:
    return "nil";
  case cell_type_e::INTEGER:
    return std::to_string(this->as_integer());
  case cell_type_e::DOUBLE:
    return std::to_string(this->as_double());
  case cell_type_e::SYMBOL:
    [[fallthrough]];
  case cell_type_e::STRING:
    return this->as_string();
  case cell_type_e::REFERENCE: {
    auto *cell = this->to_referenced_cell();
    if (!cell)
      return "nil";
    else
      return cell->to_string();
    return cell->to_string();
  }
  case cell_type_e::ABERRANT: {
    aberrant_cell_if *cell = this->as_aberrant();
    if (!cell)
      return "nil";
    else
      return cell->represent_as_string();
  }
  case cell_type_e::FUNCTION: {
    auto &fn = this->as_function_info();
    std::string result = "<function:";
    result += fn.name;
    result += ", type:";
    result += function_type_to_string(fn.type);
    result += ">";
    return result;
  }
  case cell_type_e::LIST: {
    std::string result;
    auto &list_info = this->as_list_info();

    switch (list_info.type) {
    case list_types_e::INSTRUCTION: {
      result += "(";
      for (auto *cell : list_info.list) {
        result += cell->to_string() + " ";
      }
      result.pop_back();
      result += ")";
      break;
    }
    case list_types_e::DATA: {
      result += "[";
      for (auto *cell : list_info.list) {
        result += cell->to_string() + " ";
      }
      result.pop_back();
      result += "]";
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