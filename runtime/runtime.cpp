#include "runtime.hpp"

#include "common/rang.hpp"
#include <iostream>

runtime_c *global_runtime{nullptr};

bool global_runtime_init(env_c &env, source_manager_c &source_manager) {
  if (global_runtime) {
    return true;
  }

  global_runtime = new runtime_c(env, source_manager);

  if (!global_runtime) {
    return false;
  }

  return true;
}

void global_runtime_destroy() {
  if (global_runtime) {
    delete global_runtime;
    global_runtime = nullptr;
  }
}

runtime_c::runtime_c(env_c &env, source_manager_c &source_manager)
    : global_env_(env), source_manager_(source_manager) {}

void runtime_c::on_list(cell_ptr list_cell) {
  try {
    // We can ignore the return value because
    // at this level nothing would be returned
    execute_cell(list_cell, global_env_);
  } catch (runtime_c::exception_c &error) {
    halt_with_error(error_c(error.get_source_location(), error.what()));
  } catch (cell_access_exception_c &error) {
    halt_with_error(error_c(error.get_source_location(), error.what()));
  }
}

void runtime_c::halt_with_error(error_c error) {

  /*
      TODO:
      Need to create a "halt" or "abort" object that
      the parser/ scanner/ builder all inherit an interface
      "stoppable" or something like that so when we can
      toss errors to from here, stop execution, and
      print the error message

      Once that is complete we will gracefully shutdown

  */
  std::cout << rang::fg::red << "\n[ RUNTIME HALT ]\n"
            << rang::fg::reset << std::endl;
  error.draw_error();
  std::exit(1);
}

cell_ptr runtime_c::execute_cell(cell_ptr cell, env_c &env,
                                 bool process_data_list) {

  switch (cell->type) {
  case cell_type_e::LIST: {
    auto &list_info = cell->as_list_info();

    // If its a known data list just return the list
    if (list_info.type == list_types_e::DATA) {
      if (process_data_list) {
        cell_ptr last_result = global_cell_nil;
        for (auto &list_cell : list_info.list) {
          last_result = execute_cell(list_cell, env);
        }
        return last_result;
      }

      // If we aren't processing it, just return the list
      return cell;
    }

    // The list builder doesn't allow us to get empty lists
    // so we can assume that the list is not empty

    // All lists' first item should be a function of some sort,
    // so we recurse to either load
    auto operation = list_info.list.front();
    if (operation->type == cell_type_e::SYMBOL) {

      // If the operation is a symbol then we need to
      // look it up in the environment
      operation = execute_cell(operation, env);
    }

    // If the operation is still not a function then
    // we have a problem, so we throw an error
    if (operation->type != cell_type_e::FUNCTION) {
      throw exception_c("The first item in a list must be a function",
                        operation->locator);
      return nullptr;
    }

    // All functions point to a `cell_fn_t`, even lambda functions
    // so we can just call the function and return the result
    return operation->as_function_info().fn(list_info.list, env);
  }
  case cell_type_e::SYMBOL: {
    // Load the symbol from the environment
    auto loaded_cell = env.get(cell->as_symbol());
    if (!loaded_cell) {
      throw exception_c("Symbol not found in environment: " + cell->as_symbol(),
                        cell->locator);
      return nullptr;
    }
    // Return the loaded cell

    return loaded_cell;
  }
  case cell_type_e::REFERENCE: {
    return cell->to_referenced_cell();
  }
  case cell_type_e::ABERRANT:
    [[fallthrough]];
  case cell_type_e::INTEGER:
    [[fallthrough]];
  case cell_type_e::DOUBLE:
    [[fallthrough]];
  case cell_type_e::STRING: {
    // Raw variable types can be loaded directly
    return cell;
  }
  default: {
    std::string msg = "Unhandled cell type: ";
    msg += cell_type_to_string(cell->type);
    throw exception_c(msg, cell->locator);
    return nullptr;
  }
  }
  return nullptr;
}
