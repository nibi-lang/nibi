#pragma once

#include "machine/instructions.hpp"
#include "machine/object.hpp"
#include "front/imports.hpp"
#include <queue>
#include <stack>

namespace machine {

class engine_c : public instruction_receiver_if {
public:

  /*
      Discuss state with the one sending instructions to see if the 
      item is a top leve item or not. 

      If it is, and its a function we know we can allow updates to the 
      bytecode as it executes to optimize it.

      If it isn't but its a hot-loop we should also be able to tell at 
      this stage

      - Perhaps we could send a state struct with "hints" and
        configurations about the received chunk of code


  */

  engine_c(runtime::context_c& rt_ctx, front::import_c &importer)
    : _rt_ctx(rt_ctx),
      _importer(importer),
      _scope(_rt_ctx.get_memory_core().global_var_env){
    }

  void set_print_results(bool val) { _print_result = true; }

  void handle_instructions(
    const bytes_t& instructions,
    instruction_error_handler_if& error_handler) override;

  virtual void reset_instruction_handling() override;
private:

  struct execution_ctx_s {
    const bytes_t* instructions{nullptr};
    instruction_error_handler_if* error_handler{nullptr};
    size_t pc{0};
    size_t instruction_number{0};

    std::queue<object_c> proc_q;
    std::queue<object_c> result_q;

    void reset() {
      pc = 0;
      instruction_number = 0;
      instructions = nullptr;
      error_handler = nullptr;
      proc_q = {};
      result_q = {};
    }
  };

  class scope_c {
  public:
    scope_c() = delete;
    scope_c(env_c& global_env)
      : _global(global_env){}

    [[nodiscard]] env_c* current() {
      if (_scopes.empty()) {
        return &_global;
      }
      return _scopes.top();
    };

    void push_scope() {
      _scopes.push(new env_c(current()));
    }

    void pop_scope() {
      if (_scopes.empty()) return;
      delete _scopes.top();
      _scopes.pop();
    } 

  private:
    env_c& _global;
    std::stack<env_c*> _scopes;
  };

  runtime::context_c &_rt_ctx;
  front::import_c &_importer;
  execution_ctx_s _ctx;
  scope_c _scope;
  bool _engine_okay{true};
  bool _print_result{false};

  void execute_ctx(execution_ctx_s &ctx);
  void execute(execution_ctx_s &ctx, instruction_view_s*);
};



} // namespace
