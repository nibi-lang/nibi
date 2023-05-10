#include "interpreter.hpp"

#include "libnibi/rang.hpp"
#include "libnibi/common/platform.hpp"
#include <iostream>

#if PROFILE_INTERPRETER
#include <chrono>
#endif

namespace nibi {

interpreter_c *global_interpreter{nullptr};

bool global_interpreter_init(env_c &env, source_manager_c &source_manager) {
  if (global_interpreter) {
    return true;
  }

  global_interpreter = new interpreter_c(env, source_manager);

  if (!global_interpreter) {
    return false;
  }

  return true;
}

void global_interpreter_destroy() {
  if (global_interpreter) {
    delete global_interpreter;
    global_interpreter = nullptr;
  }
}

interpreter_c::interpreter_c(env_c &env, source_manager_c &source_manager)
    : global_env_(env), source_manager_(source_manager) {}

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

void interpreter_c::on_list(cell_ptr list_cell) {
  try {
    // We can ignore the return value because
    // at this level nothing would be returned
    execute_cell(list_cell, global_env_);
  } catch (interpreter_c::exception_c &error) {
    halt_with_error(error_c(error.get_source_location(), error.what()));
  } catch (cell_access_exception_c &error) {
    halt_with_error(error_c(error.get_source_location(), error.what()));
  }
}

void interpreter_c::halt_with_error(error_c error) {

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

cell_ptr interpreter_c::execute_cell(cell_ptr cell, env_c &env,
                                     bool process_data_list) {

  if (yield_value_) {
    return yield_value_;
  }

  if (!cell) {
    return allocate_cell(cell_type_e::NIL);
  }

  switch (cell->type) {
  case cell_type_e::LIST: {
    return handle_list_cell(cell, env, process_data_list);
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
  case cell_type_e::ABERRANT:
    [[fallthrough]];
  case cell_type_e::ENVIRONMENT:
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

  auto &list_info = cell->as_list_info();

  switch (list_info.type) {
  case list_types_e::DATA: {
    if (process_data_list) {
      cell_ptr last_result = allocate_cell(cell_type_e::NIL);
      for (auto &list_cell : list_info.list) {
        last_result = execute_cell(list_cell, env);
        if (this->is_yielding()) {
          return yield_value_;
        }
      }
      return last_result;
    }

    // If we aren't processing it, just return the list
    break;
  }
  case list_types_e::ACCESS: {
    // Each item in a list should be a symbol
    // and directing us through environments to a final cell value
    auto it = list_info.list.begin();
    auto *current_env = &env;
    cell_ptr result = allocate_cell(cell_type_e::NIL);
    for (std::size_t i = 0; i < list_info.list.size() - 1; i++) {
      result = execute_cell(*it, *current_env);
      if (result->type == cell_type_e::ENVIRONMENT) {
        current_env = result->as_environment_info().env;
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
    return execute_cell(*it, *current_env);
  }
  case list_types_e::INSTRUCTION: {
    // All lists' first item should be a function of some sort,
    // so we recurse to either load
    auto operation = list_info.list.front();
    if (operation->type == cell_type_e::SYMBOL) {

      // If the operation is a symbol then we need to
      // look it up in the environment
      operation = execute_cell(operation, env);
    }

    // If the operation is a list then we need to
    // execute it to get the function
    if (operation->type == cell_type_e::LIST) {
      operation = handle_list_cell(operation, env, true);

      // Now that its loaded, we need to update the first item
      // to be the loaded function
      list_info.list.front() = operation;
    }

    auto fn_info = operation->as_function_info();
#if PROFILE_INTERPRETER
    if (fn_call_data_.find(fn_info.name) == fn_call_data_.end()) {
      fn_call_data_[fn_info.name] = {0, 0};
    }
    auto start = std::chrono::high_resolution_clock::now();
    auto value = fn_info.fn(list_info.list, env);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start)
            .count();
    auto &t = fn_call_data_[fn_info.name];
    t.time += duration;
    t.calls++;

    return value;
#else
    // All functions point to a `cell_fn_t`, even lambda functions
    // so we can just call the function and return the result
    return fn_info.fn(list_info.list, env);
#endif
  }
  }

  // If we get here then we have a list that is not a function
  // so we return it as is
  return cell;
}

void interpreter_c::load_module(cell_ptr &module_name) {

  std::cout << "use module: " << module_name->as_string() << std::endl;

  std::string name = module_name->as_string();

  // check if its already loaded
  if (_loaded_modules.contains(name)) {
    return;
  }

  auto opt_path = global_platform->locate_directory(name);

  if (!opt_path.has_value()) {
    halt_with_error(
      error_c(module_name->locator, "Could not locate module: " + name)
    );
  }

  // load the module

  auto path = opt_path.value();

  std::cout << "loading module at: " << path << std::endl;

  auto module_file = path / "mod.nibi";

  std::cout << "Looking for module file: " << module_file << std::endl;

  if (!std::filesystem::exists(module_file)) {
    halt_with_error(
      error_c(module_name->locator, "Could not locate module file: " + module_file.string())
    );
  }

  if (!std::filesystem::is_regular_file(module_file)) {
    halt_with_error(
      error_c(module_name->locator, "Module file is not a regular file: " + module_file.string())
    );
  }

  env_c module_env;
  source_manager_c module_source_manager;
  interpreter_c module_interpreter(module_env, module_source_manager);
  list_builder_c module_builder(module_interpreter);
  file_reader_c module_reader(module_builder, module_source_manager);
  module_reader.read_file(module_file.string());

  // Env that everything will be dumped into and 
  // then put into a cell
  environment_info_s module_cell_env = {name, new env_c()};

  bool loaded_something{false};

  auto dylib = module_env.get("dylib");
  if (nullptr != dylib) {
    load_dylib(name, *module_cell_env.env, path, dylib);
    loaded_something = true;
  }

  auto source_list = module_env.get("sources");
  if (nullptr != source_list) {
    load_source_list(name, *module_cell_env.env, path, source_list);
    loaded_something = true;
  }

  if (!loaded_something) {
    halt_with_error(
      error_c(module_name->locator, "Module did not contain any loadable items")
    );
  }

  auto new_env_cell = allocate_cell(module_cell_env);
  global_env_.set(name, new_env_cell);
}

inline void interpreter_c::load_dylib(
  std::string &name, 
  env_c &module_env,
  std::filesystem::path &module_path,
  cell_ptr &dylib_cell) {

  std::cout << "\nNeed to load dylib for module: " << name << std::endl;
  std::cout << dylib_cell->to_string() << std::endl;
}

inline void interpreter_c::load_source_list(
  std::string &name,
  env_c &module_env,
  std::filesystem::path &module_path,
  cell_ptr &source_list) {

  auto source_list_info = source_list->as_list_info();

  // Create an interpreter/ builder/ reader that will dump 
  // all the source into the module env
  interpreter_c module_interpreter(module_env, source_manager_);
  list_builder_c module_builder(module_interpreter);
  file_reader_c module_reader(module_builder, source_manager_);

  // Walk over each file given by the source list
  // and read it into the module env by way of the new interpreter
  for (auto& source_file : source_list_info.list) {
    auto source_file_path = module_path / source_file->as_string();
    if (!std::filesystem::is_regular_file(source_file_path)) {
      halt_with_error(
        error_c(source_file->locator, 
          "File listed in module: " + 
          name + 
          " is not a regular file: " + source_file_path.string())
      );
    }
    module_reader.read_file(source_file_path.string());
  }
}

}