#pragma once

#include "runtime/context.hpp"
#include "runtime/object.hpp"
#include "runtime/env.hpp"
#include <functional>

namespace cppmod {


struct callable_s {
  std::string command;
  machine::cpp_fn fn;
};


// TODO: Make an interface for all modules to register their
//        respective keywords as CPPFN object_c items

class module_if {
public:
  module_if(const std::string& name, runtime::context_c &ctx)
    : _name(name), _ctx(ctx) {}

  std::string get_name() const { return _name; }

  virtual machine::object_c call(machine::object_list& list, machine::env_c* env) = 0;



protected:

  void register_call(callable_s &call) {
    
  }

  void raise_error(const std::string& msg, const bool& is_fatal) {
    _ctx.error_handler->on_error(
      ctx.instruction_number,
      machine::execution_error_s{msg, is_fatal});
  }

private:
  std::string _name;
  runtime::context_c &_ctx;

  std::vector<callable_s
};

} // namespace
