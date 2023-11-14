#include "machine/engine.hpp"
#include "machine/byte_tools.hpp"
#include <fmt/format.h>

#include <iostream> // TODO: remove

namespace machine {

#define RAISE_ERROR(s) \
  print_instruction_data(ctx.instructions->data(), ctx.instructions->size()); \
  ctx.error_handler->on_error( \
    ctx.instruction_number, \
    execution_error_s{s, true}); \
  _engine_okay = false; \
  return;

void engine_c::reset_instruction_handling() {

  if (_print_result) {
    while(!_ctx.result_q.empty()) {
      fmt::print("{} ", _ctx.result_q.front().dump_to_string(true));
      _ctx.result_q.pop();
      fmt::print("\n");
    }
  }

  _engine_okay = true;
  _ctx.reset();
}

void engine_c::handle_instructions(
  const bytes_t& instructions,
  instruction_error_handler_if& error_handler) {

  _ctx.instructions = &instructions;
  _ctx.error_handler = &error_handler;

  execute_ctx(_ctx);
}

void engine_c::execute_ctx(execution_ctx_s &ctx) {

  //print_instruction_data(ctx.instructions->data(), ctx.instructions->size()); 

  const bytes_t& ins = *ctx.instructions;

  while(_engine_okay && 
        ctx.pc < ctx.instructions->size()) {

    auto& current_byte = ins[ctx.pc];

    if (current_byte >= (uint8_t)ins_id_e::ENUM_BOUNDARY) {
      RAISE_ERROR(fmt::format("Invalid instruction `{}`\n", (int)current_byte));
    }

    instruction_view_s* iv =
      (instruction_view_s*)
      ((uint8_t*)(ins.data() + ctx.pc));

    execute(ctx, iv);

    ctx.pc += FIELD_OP_SIZE_BYTES;
    if (current_byte >= INS_DATA_BOUNDARY) {
      ctx.pc += FIELD_DATA_LEN_SIZE_BYTES + iv->data_len;
    }

    ctx.instruction_number++;
  }
}

#define COND_LOAD_OR_FAIL(target_) \
  if (!(target_.conditional_self_load(_scope.current()))) {\
      RAISE_ERROR(fmt::format("Unknown variable `{}`\n", target_.dump_to_string(true))) \
  } \

#define BINARY_OP(operation) \
{ \
  if (ctx.proc_q.size() != 2) {\
    RAISE_ERROR(\
      fmt::format( \
        "Binary operation expects exactly 2 parameters, {} were given",\
        ctx.proc_q.size()));\
  } \
  auto lhs = ctx.proc_q.front(); \
  ctx.proc_q.pop(); \
  auto rhs = ctx.proc_q.front(); \
  ctx.proc_q.pop(); \
  COND_LOAD_OR_FAIL(lhs)\
  COND_LOAD_OR_FAIL(rhs)\
  ctx.proc_q.push(lhs operation rhs);\
  break; \
}

#define MATH_OP(operation) \
{\
  object_c result = ctx.proc_q.front(); \
  ctx.proc_q.pop(); \
  COND_LOAD_OR_FAIL(result)\
  while(!ctx.proc_q.empty()) { \
    auto* next = &ctx.proc_q.front(); \
    COND_LOAD_OR_FAIL((*next))\
    result = result operation (*next); \
    ctx.proc_q.pop(); \
  } \
  ctx.proc_q.push(result); \
  break;\
  }

#define FOR_ALL_ITEMS(body_) \
  while(!ctx.proc_q.empty()) {\
    auto item = ctx.proc_q.front(); \
    ctx.proc_q.pop(); \
    body_ \
  }

#define ASSERT_TYPE(item_, type_) \
  if (item_.type != type_) { \
    RAISE_ERROR( \
      fmt::format(\
        "Expected object type '{}' got '{}'\n",\
         data_type_to_string(type_),\
         data_type_to_string(item_.type)));\
  }

#define LOAD(var_) \
 if (var_.type != data_type_e::IDENTIFIER) {\
    RAISE_ERROR(\
      fmt::format("Expected identifier, got '{}'\n",\
        data_type_to_string(var_.type)));\
  }\
  if (!var_.conditional_self_load(_scope.current())) { \
     RAISE_ERROR(fmt::format("Unknown variable '{}'\n", var_.to_string()));\
 }


void engine_c::execute(execution_ctx_s &ctx,instruction_view_s* iv) {

  //if (iv->op >= INS_DATA_BOUNDARY) {
  //  fmt::print("Op data len : {}\n", iv->data_len);
  //}
  //fmt::print("op: {} | idx: {}\n", ins_to_string((ins_id_e)iv->op), ctx.instruction_number);

  switch((ins_id_e)iv->op) {
    case ins_id_e::NOP: 
      fmt::print("NOP\n");
      break;
    case ins_id_e::EXEC_ADD: MATH_OP(+);
    case ins_id_e::EXEC_SUB: MATH_OP(-);
    case ins_id_e::EXEC_DIV: MATH_OP(/);
    case ins_id_e::EXEC_MUL: MATH_OP(*);
    case ins_id_e::EXEC_MOD: BINARY_OP(%);
    case ins_id_e::EXEC_EQ: {
      auto lhs = ctx.proc_q.front();
      ctx.proc_q.pop();
      auto rhs = ctx.proc_q.front();
      ctx.proc_q.pop();
      COND_LOAD_OR_FAIL(lhs)
      COND_LOAD_OR_FAIL(rhs)
      ctx.proc_q.push(lhs.equals(rhs));
      break;
    }
    case ins_id_e::EXEC_NE: {
      auto lhs = ctx.proc_q.front();
      ctx.proc_q.pop();
      auto rhs = ctx.proc_q.front();
      ctx.proc_q.pop();
      COND_LOAD_OR_FAIL(lhs)
      COND_LOAD_OR_FAIL(rhs)
      ctx.proc_q.push(lhs.not_equals(rhs));
      break;
    }
    case ins_id_e::EXEC_LT: {
      auto lhs = ctx.proc_q.front();
      ctx.proc_q.pop();
      auto rhs = ctx.proc_q.front();
      ctx.proc_q.pop();
      COND_LOAD_OR_FAIL(lhs)
      COND_LOAD_OR_FAIL(rhs)
      ctx.proc_q.push(lhs.is_less_than(rhs));
      break;
    }
    case ins_id_e::EXEC_GT: {
      auto lhs = ctx.proc_q.front();
      ctx.proc_q.pop();
      auto rhs = ctx.proc_q.front();
      ctx.proc_q.pop();
      COND_LOAD_OR_FAIL(lhs)
      COND_LOAD_OR_FAIL(rhs)
      ctx.proc_q.push(lhs.is_greater_than(rhs));
      break;
    }
    case ins_id_e::EXEC_ASSERT: {
      if (ctx.proc_q.front().to_integer() >= 1) {
        // Don't remove value so we can chain them
        break;
      }
      if (iv->data_len) {
        std::string message((char*)(iv->data), iv->data_len);
        RAISE_ERROR(fmt::format("Assertion failure: {}", message));
      }
      RAISE_ERROR("Assertion");
      break;
    }
    case ins_id_e::EXEC_IDENTIFIER: {
      std::string name((char*)(iv->data), iv->data_len);
      auto* target_call = _scope.current()->get(name);
      if (target_call == nullptr) {
        RAISE_ERROR(
          fmt::format("Call to unknown function '{}'", name));
      }
      object_cpp_fn_c* cppfn = target_call->as_cpp_fn();
      if (cppfn == nullptr) {
        RAISE_ERROR(fmt::format("Object of non-cppfn can not yet be called.. NYD"));
      }
      return call_cpp_fn(cppfn, ctx);
    }
    case ins_id_e::EXEC_DBG: {
      FOR_ALL_ITEMS({
        fmt::print("{}", item.dump_to_string(true));
        if (item.type == data_type_e::IDENTIFIER) {
          if (!item.conditional_self_load(_scope.current())) {
            fmt::print(" = 'unset'");
          } else {
            fmt::print(" = '{}'", item.dump_to_string(true));
          }
        }
        fmt::print("\n");
      })
      break;
    }
    case ins_id_e::EXEC_IMPORT: {
      FOR_ALL_ITEMS({
        ASSERT_TYPE(item, data_type_e::STRING)
        std::filesystem::path target_path(item.to_string());
        if (!_importer.import_file(
              target_path,
              _rt_ctx.get_working_dir())) {
          RAISE_ERROR(
            fmt::format("Failed to locate file for import '{}'",
              item.to_string()));
        }
      })
      break;
    }
    case ins_id_e::EXEC_ASSIGN: {
      auto new_var = ctx.proc_q.front();
      ctx.proc_q.pop();

      if (_scope.current()->get(new_var.to_string(), true)) {
        RAISE_ERROR(fmt::format(
          "Variable '{}' in assignment already exists in current scope\n",
          new_var.to_string()));
      }

      auto* val = &ctx.proc_q.front();
      if (val->type == data_type_e::IDENTIFIER && 
         !val->conditional_self_load(_scope.current())) {
          RAISE_ERROR(fmt::format("Unknown variable '{}' in assignment\n", val->to_string()));
      }

      _scope.current()->insert(
          new_var.to_string(),
          ctx.proc_q.front());

      ctx.proc_q.pop();

      // TODO:
      // Push a reference to the stored variable so we _could_ chain assignments
      break;
    }
    case ins_id_e::EXEC_REASSIGN: {
      auto var = ctx.proc_q.front();
      ctx.proc_q.pop();

      std::string name = var.to_string();

      auto* target = _scope.current()->get(name);

      if (!target) {
        RAISE_ERROR(fmt::format("Unknown variable '{}' in reassignment\n", name));
      }
    
      target->update_from(ctx.proc_q.front());
      ctx.proc_q.pop();

      fmt::print("Reassigned {} as {}\n", name, target->dump_to_string(true));
      // Push a reference to the stored variable
      break;
    }
    case ins_id_e::EXPECT_OBJECT_TYPE: {
      data_type_e expected = (data_type_e)((uint8_t)(*(uint8_t*)(iv->data)));
      ASSERT_TYPE(ctx.proc_q.front(), expected)
      break;
    }
    case ins_id_e::EXPECT_N_ARGS: {
      uint8_t expected_size = *(uint8_t*)(iv->data);
      if (ctx.proc_q.size() != expected_size) {
        RAISE_ERROR(
          fmt::format(
           "Expected exactly '{}' arguments. Got '{}'\n", 
           expected_size, ctx.proc_q.size()));
      }
      break;
    }
    case ins_id_e::EXPECT_GTE_N_ARGS: {
      uint8_t expected_size = *(uint8_t*)(iv->data);
      if (ctx.proc_q.size() < expected_size) {
        RAISE_ERROR(
          fmt::format(
            "Expected at least '{}' arguments. Got '{}'\n",
            expected_size, ctx.proc_q.size()));
      }
      break;
    }
    case ins_id_e::SAVE_RESULTS: {
      while(!ctx.proc_q.empty()) {
        ctx.result_q.push(ctx.proc_q.front());
        ctx.proc_q.pop();
      }
      break;
    }
    case ins_id_e::PUSH_RESULT: {
      if (ctx.result_q.empty()) {
        ctx.proc_q.push(object_c::integer(0));
        break;
      }
      ctx.proc_q.push(ctx.result_q.front());
      ctx.result_q.pop();
      break;
    }
    case ins_id_e::PUSH_INT:
      ctx.proc_q.push(
        object_c::integer(*(int64_t*)(iv->data)));
      break;
    case ins_id_e::PUSH_REAL:
      ctx.proc_q.push(
        object_c::real(*(double*)(iv->data)));
      break;
    case ins_id_e::PUSH_IDENTIFIER:
      ctx.proc_q.push(
        object_c::identifier((char*)(iv->data), iv->data_len));
      break;
    case ins_id_e::PUSH_STRING:
      ctx.proc_q.push(
        object_c::str((char*)(iv->data), iv->data_len));
      break;
    default: {
      std::cout << "OP:" << (int)iv->op << " not implemented yet\n";
    }
  }

  // TODO: We could, maybe _should_ add an error handler means
  //       so that some classifications of errors (like this one)
  //       can be handled by the user
  if (ctx.result_q.size() &&
      ctx.result_q.front().type == data_type_e::ERROR) {
    RAISE_ERROR(ctx.result_q.front().to_string())
  }
}

void engine_c::call_cpp_fn(object_cpp_fn_c* fnd, execution_ctx_s &ctx) {
  std::vector<object_c> params;
  FOR_ALL_ITEMS({
    if (item.type == data_type_e::IDENTIFIER) {
      COND_LOAD_OR_FAIL(item);
    }
    params.push_back(item);
  })
  fnd->fn(params, *_scope.current());
}


} // namespace
