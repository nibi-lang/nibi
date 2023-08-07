
#include "interpreter/builtins/builtins.hpp"
#include "interpreter/interpreter.hpp"
#include "libnibi/cell.hpp"
#include "libnibi/front/file_interpreter.hpp"
#include "libnibi/keywords.hpp"
#include "macros.hpp"
#include "platform.hpp"

namespace nibi {
namespace builtins {

cell_ptr builtin_fn_common_clone(interpreter_c &ci, cell_list_t &list,
                                 env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::CLONE, ==, 2)

  auto it = list.begin();
  std::advance(it, 1);

  auto loaded_cell = ci.process_cell(*it, env);

  return loaded_cell->clone(env);
}

cell_ptr builtin_fn_common_len(interpreter_c &ci, cell_list_t &list,
                               env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::LEN, ==, 2)

  auto target_list = ci.process_cell(list[1], env);

  if (target_list->type != cell_type_e::LIST) {
    return allocate_cell((int64_t)(target_list->to_string(false, true).size()));
  }

  auto &list_info = target_list->as_list_info();
  return allocate_cell((int64_t)list_info.list.size());
}

cell_ptr builtin_fn_common_exchange(interpreter_c &ci, cell_list_t &list,
                                    env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::EXCHANGE, ==, 3)
  auto target = ci.process_cell(list[1], env);
  auto source = ci.process_cell(list[2], env);
  auto result = target->clone(env);
  target->update_from(*source, env);
  return result;
}

cell_ptr builtin_fn_common_defer(interpreter_c &ci, cell_list_t &list,
                                 env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::DEFER, >=, 2)

  for (auto it = list.begin() + 1; it != list.end(); ++it) {
    if ((*it)->type != cell_type_e::LIST) {
      throw interpreter_c::exception_c(
          "Defer command requires all parameters to be of type LIST",
          (*it)->locator);
    }
    ci.defer_execution(*it);
  }
  return allocate_cell(cell_type_e::NIL);
}

cell_ptr builtin_fn_common_yield(interpreter_c &ci, cell_list_t &list,
                                 env_c &env) {

  if (list.size() == 1) {
    ci.set_yield_value(allocate_cell((int64_t)0));
    return ci.get_yield_value();
  }

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::YIELD, ==, 2)

  auto target = ci.process_cell(list[1], env)->clone(env);
  ci.set_yield_value(target);
  return target;
}

cell_ptr builtin_fn_common_loop(interpreter_c &ci, cell_list_t &list,
                                env_c &env) {
  // (loop (pre) (cond) (post) (body))
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::LOOP, ==, 5)

  auto it = list.begin();
  std::advance(it, 1);

  auto pre_condition = (*it);
  std::advance(it, 1);

  auto condition = (*it);
  std::advance(it, 1);

  auto post_condition = (*it);
  std::advance(it, 1);

  auto body = (*it);

  ci.push_ctx();

  auto loop_env = env_c(&env);

  ci.process_cell(pre_condition, loop_env);

  cell_ptr result = allocate_cell(cell_type_e::NIL);
  while (true) {
    auto condition_result = ci.process_cell(condition, loop_env);

    if (condition_result->to_integer() <= 0) {
      return result;
    }

    result = ci.process_cell(body, loop_env, true);

    if (ci.is_yielding()) {
      return ci.get_yield_value();
    }

    ci.process_cell(post_condition, loop_env);
  }

  ci.pop_ctx(loop_env);

  return result;
}

cell_ptr builtin_fn_common_if(interpreter_c &ci, cell_list_t &list,
                              env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::IF, >=, 3)

  auto it = list.begin();
  std::advance(it, 1);

  auto condition = (*it);
  std::advance(it, 1);

  auto true_condition = (*it);

  auto if_env = env_c(&env);

  ci.push_ctx();

  auto condition_result = ci.process_cell(condition, if_env);

  if (condition_result->as_integer() > 0) {
    ci.pop_ctx(if_env);
    return ci.process_cell(true_condition, if_env, true);
  }

  if (list.size() == 4) {
    std::advance(it, 1);
    ci.pop_ctx(if_env);
    return ci.process_cell((*it), if_env, true);
  }
  ci.pop_ctx(if_env);

  return ci.get_last_result();
}

cell_ptr builtin_fn_common_import(interpreter_c &ci, cell_list_t &list,
                                  env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::IMPORT, >=, 2)

  auto it = list.begin();
  std::advance(it, 1);

  auto &gsm = ci.get_source_manager();

  error_callback_f error_callback = [&](error_c error) {
    error.draw();
    throw interpreter_c::exception_c("Import error");
  };

  while (it != list.end()) {
    // Get the source file from the `import` keyword so we can ensure that
    // the file can be searched relative to the location of the file being
    // executed at the moment
    auto from =
        std::filesystem::path((*list.begin())->locator->get_source_name());

    // Retrieve the file name to import
    auto target = std::filesystem::path((*it)->as_string());

    // Locate the item
    auto item = global_platform->locate_file(target, from);

    if (!item.has_value()) {
      throw interpreter_c::exception_c("Could not locate file for import: " +
                                           target.string(),
                                       (*it)->locator);
    }

    // Check that the item hasn't already been imported
    if (!gsm.exists((*item).string())) {
      file_interpreter_c(error_callback, ci.get_env(), gsm)
          .interpret_file((*item).string());
    }
    std::advance(it, 1);
  }
  return allocate_cell((int64_t)1);
}

cell_ptr builtin_fn_common_use(interpreter_c &ci, cell_list_t &list,
                               env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::USE, >=, 2)

  auto it = list.begin();
  std::advance(it, 1);

  while (it != list.end()) {
    ci.load_module((*it));
    std::advance(it, 1);
  }
  return allocate_cell((int64_t)1);
}

cell_ptr builtin_fn_common_exit(interpreter_c &ci, cell_list_t &list,
                                env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::EXIT, ==, 2)
  auto it = list.begin();
  std::advance(it, 1);
  std::exit(ci.process_cell((*it), env)->as_integer());
}

cell_ptr builtin_fn_common_quote(interpreter_c &ci, cell_list_t &list,
                                 env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::QUOTE, ==, 2)
  auto it = list.begin();
  std::advance(it, 1);
  return allocate_cell((*it)->to_string(false, true));
}

cell_ptr builtin_fn_common_eval(interpreter_c &ci, cell_list_t &list,
                                env_c &env) {
  NIBI_LIST_ENFORCE_SIZE(nibi::kw::EVAL, ==, 2)
  auto it = list.begin();
  std::advance(it, 1);

  auto sm = ci.get_source_manager();
  auto so = sm.get_source(list[0]->locator->get_source_name());

  // Intercept all instructions generated by intake_c
  // and store them in a vector
  class catcher_c : public instruction_processor_if {
  public:
    catcher_c(std::vector<cell_ptr> &ins) : ins_(ins) {}
    void instruction_ind(cell_ptr &cell) override { ins_.push_back(cell); }

  public:
    std::vector<cell_ptr> &ins_;
  };

  std::vector<cell_ptr> instructions;
  instructions.reserve(100);

  catcher_c ins_catch(instructions);

  // Evaluate the source code
  intake_c(
      ins_catch,
      [&](error_c error) {
        error.draw();
        throw interpreter_c::exception_c("Eval error");
      },
      sm, builtins::get_builtin_symbols_map())
      .evaluate(ci.process_cell((*it), env)->as_string(), so, list[0]->locator);

  // Process the instructions
  cell_ptr result = allocate_cell(cell_type_e::NIL);
  for (auto &ins : instructions) {
    result = ci.process_cell(ins, env);
  }
  return result;
}

cell_ptr builtin_fn_common_nop(interpreter_c &ci, cell_list_t &list,
                               env_c &env) {
  return allocate_cell(cell_type_e::NIL);
}

cell_ptr assemble_macro(interpreter_c &ci, cell_list_t &list, env_c &env) {

  auto definition = env.get(list[0]->as_symbol());
  auto macro_env = definition->as_function_info().operating_env;
  auto macro_params = macro_env->get("$params")->as_list_info();
  auto macro_body = macro_env->get("$body")->as_string();

  if (list.size() != macro_params.list.size() + 1) {
    throw interpreter_c::exception_c(
        std::string("Macro `") + list[0]->as_symbol() + "` expected " +
            std::to_string(macro_params.list.size()) + " parameters, but " +
            std::to_string(list.size() - 1) + " were given",
        list[0]->locator);
  }

  for (std::size_t i = 0; i < macro_params.list.size(); i++) {
    auto &param = macro_params.list[i];
    auto &arg = list[i + 1];
    auto target_replacement = "%" + param->as_symbol();
    auto found = macro_body.find(target_replacement);
    while (found != std::string::npos) {
      if (found > 0 && macro_body[found - 1] == '\\') {
        found = macro_body.find(target_replacement, found + 1);
        continue;
      }
      macro_body.replace(found, target_replacement.size(),
                         arg->to_string(true, true));
      found = macro_body.find(target_replacement);
    }
  }

  cell_list_t eval_list;
  eval_list.emplace_back(macro_env->get("$body_start"));
  eval_list.emplace_back(allocate_cell(macro_body));
  return builtin_fn_common_eval(ci, eval_list, env);
}

cell_ptr builtin_fn_common_macro(interpreter_c &ci, cell_list_t &list,
                                 env_c &env) {

  NIBI_LIST_ENFORCE_SIZE(nibi::kw::MACRO, >=, 4)

  /*
     For macros, we will construct a callable function that has an owned
     cell environment.

     We will store the parameter list into the
     function enviornment wholesale as they should just be symbols
     that will be replaced in the body of the macro.

     We will also store the function body in this environment
     as a quoted list.

     Once we create this function object we will store it in
     the current operating enviornment as the macro_name.

     This will allow us to not modify any other part of the code base
     to gain macro functionality. When a macro is called,
     the call will be interpreted as a function pointing to the
     function above (assemble_macro). From there,
     the macro will be assembled and then executed in
     the environment that is given.
   */

  auto macro_name = list[1]->as_symbol();

  NIBI_VALIDATE_VAR_NAME(macro_name, list[1]->locator);

  // Expect param list even it its empty

  auto params = list[2]->as_list_info();

  if (params.type != list_types_e::DATA) {
    throw interpreter_c::exception_c(
        "Macro parameters are expected to be a data list '[]'",
        list[2]->locator);
  }

  function_info_s macro_assembler_fn("assemble_macro", assemble_macro,
                                     function_type_e::FAUX, new env_c());

  macro_assembler_fn.operating_env->set("$params", list[2]);

  // Everything else should be considered part of the body
  // and should be quoted and concatenated

  auto it = list.begin();
  std::advance(it, 3);

  auto body_start = (*it);

  std::string macro_string_body;
  while (it != list.end()) {
    macro_string_body += (*it)->to_string(true, true);
    std::advance(it, 1);
  }

  auto body = allocate_cell(macro_string_body);

  macro_assembler_fn.operating_env->set("$body", body);
  macro_assembler_fn.operating_env->set("$body_start", body_start);

  auto resulting_macro = allocate_cell(macro_assembler_fn);

  env.set(macro_name, resulting_macro);

  return allocate_cell((int64_t)1);
}

} // namespace builtins
} // namespace nibi
