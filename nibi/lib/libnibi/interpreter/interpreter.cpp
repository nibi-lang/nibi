#include "interpreter.hpp"

#include "libnibi/platform.hpp"
#include "libnibi/rang.hpp"

#if PROFILE_INTERPRETER
#include <chrono>
#include <iostream>
#endif

namespace nibi {

interpreter_c::interpreter_c(env_c &env, source_manager_c &source_manager)
    : interpreter_env(env), source_manager_(source_manager),
      modules_(source_manager, *this) {
  last_result_ = allocate_cell(cell_type_e::NIL);
}

interpreter_c::~interpreter_c() {
#if PROFILE_INTERPRETER

  for (auto [fn, data] : fn_call_data_) {
    if (data.calls == 0) {
      data.calls = 1;
    }
    std::cout << fn << " called " << data.calls << " times, took " << data.time
              << " micro seconds" << std::endl;
    std::cout << "Average time: " << (data.time / data.calls)
              << " micro seconds" << std::endl;
    std::cout << "----------------" << std::endl;
  }

#endif
}

void interpreter_c::instruction_ind(cell_ptr &cell) {
  try {
    // We can ignore the return value because
    // at this level nothing would be returned
    last_result_ = handle_list_cell(cell, interpreter_env, false);
  } catch (interpreter_c::exception_c &error) {
    halt_with_error(error_c(error.get_source_location(), error.what()));
  } catch (cell_access_exception_c &error) {
    halt_with_error(error_c(error.get_source_location(), error.what()));
  } catch (std::exception &error) {
    halt_with_error(error_c(cell->locator, error.what()));
  }
}

void interpreter_c::halt_with_error(error_c error) {

  // We don't want to halt in repl mode. Just draw the error and keep truckin
  if (repl_mode_) {
    error.draw();
    return;
  }

  std::cout << rang::fg::red << "\n[ RUNTIME HALT ]\n"
            << rang::fg::reset << std::endl;
  error.draw();

  std::cout << rang::fg::yellow << "\n[ CALL TRACE ]\n"
            << rang::fg::reset << std::endl;

  // Print the stack trace
  while (!call_stack_.empty()) {
    auto top_cell = call_stack_.top();

    std::cout << ">>> " << rang::fg::cyan << top_cell->to_string(true, true)
              << rang::fg::reset;

    if (top_cell->locator) {
      std::cout << " in " << top_cell->locator->get_source_name() << ":("
                << top_cell->locator->get_line() << ":"
                << top_cell->locator->get_column() << ")";
    } else {
      std::cout << " in <location unknown>";
    }

    std::cout << std::endl;

    call_stack_.pop();
  }

  std::exit(1);
}

cell_ptr interpreter_c::process_cell(cell_ptr cell, env_c &env,
                                     const bool process_data_list) {

  if (yield_value_) {
    return yield_value_;
  }

  if (!cell) {
    return allocate_cell(cell_type_e::NIL);
  }

  switch (cell->type) {
  case cell_type_e::LIST: {
    return std::move(handle_list_cell(cell, env, process_data_list));
  }
  case cell_type_e::SYMBOL: {
    // Load the symbol from the environment
    auto loaded_cell = env.get(cell->as_symbol());
    if (!loaded_cell) {

      const std::string error =
          "Symbol not found in environment: " + cell->as_symbol();
      throw exception_c(error, cell->locator);
      return nullptr;
    }

    // Cache the cell so we don't load it again later (potentially)
    cell = loaded_cell;

    [[fallthrough]];
  }
  default: {
    return std::move(cell);
  }
  }
  return nullptr;
}

inline bool considered_private(cell_ptr &cell) {
  switch (cell->type) {
  case cell_type_e::SYMBOL: {
    return cell->as_symbol().starts_with("_");
  }
  case cell_type_e::FUNCTION: {
    return cell->as_function_info().name.starts_with("_");
  }
  case cell_type_e::ENVIRONMENT: {
    return cell->as_environment_info().name.starts_with("_");
  }
  }
  return false;
}

inline cell_ptr interpreter_c::handle_list_cell(cell_ptr &cell, env_c &env,
                                                bool process_data_list) {
  auto &list = cell->as_list();
  if (!list.size()) {
    return std::move(cell);
  }

  switch (cell->as_list_info().type) {
  case list_types_e::DATA: {
    if (process_data_list) {
      cell_ptr last_result = allocate_cell(cell_type_e::NIL);
      for (auto &list_cell : list) {
        last_result = process_cell(list_cell, env);
        if (this->is_yielding()) {
          return yield_value_;
        }
      }
      return std::move(last_result);
    }
    break;
  }
  case list_types_e::ACCESS: {
    // Each item in a list should be a symbol
    // and directing us through environments to a final cell value
    auto it = list.begin();
    auto *current_env = &env;
    cell_ptr result = allocate_cell(cell_type_e::NIL);
    for (std::size_t i = 0; i < list.size() - 1; i++) {
      result = process_cell(*it, *current_env);
      if (result->type == cell_type_e::ENVIRONMENT) {
        current_env = result->as_environment_info().env.get();
        if (considered_private(result) && i != 0) {
          halt_with_error(error_c(cell->locator,
                                  "Private members can only be accessed "
                                  "from the root of an access list"));
        }
        std::advance(it, 1);
        continue;
      }
      halt_with_error(error_c(cell->locator,
                              "Each member up-to the end of an access list "
                              "must be an environment"));
    }
    if (considered_private((*it))) {
      halt_with_error(error_c(cell->locator,
                              "Private members can only be accessed from "
                              "the root of an access list"));
    }

    cell = process_cell(*it, *current_env);
    return std::move(cell);
  }
  case list_types_e::INSTRUCTION: {
    // All lists' first item should be a function of some sort,
    // so we recurse to either load
    auto operation = list.front();
    if (operation->type == cell_type_e::SYMBOL) {

      // If the operation is a symbol then we need to
      // look it up in the environment
      operation = process_cell(std::move(operation), env);
    }

    // If the operation is a list then we need to
    // execute it to get the function
    if (operation->type == cell_type_e::LIST) {
      operation = handle_list_cell(operation, env, true);

      // Now that its loaded, we need to update the first item
      // to be the loaded function
      list.front() = operation;
    }

    auto fn_info = operation->as_function_info();

    call_stack_.push(list.front());

#if PROFILE_INTERPRETER
    if (fn_call_data_.find(fn_info.name) == fn_call_data_.end()) {
      fn_call_data_[fn_info.name] = {0, 0};
    }
    auto start = std::chrono::high_resolution_clock::now();
    auto value = fn_info.fn(*this, list, env);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start)
            .count();
    auto &t = fn_call_data_[fn_info.name];
    t.time += duration;
    t.calls++;

    call_stack_.pop();
    return value;
#else
    // All functions point to a `cell_fn_t`, even lambda functions
    // so we can just call the function and return the result
    auto value = fn_info.fn(*this, list, env);

    call_stack_.pop();
    return std::move(value);
#endif
  }
  }

  // If we get here then we have a list that is not a function
  // so we return it as is
  return std::move(cell);
}

void interpreter_c::load_module(cell_ptr &module_name) {
  modules_.load_module(module_name, interpreter_env);
}

} // namespace nibi
