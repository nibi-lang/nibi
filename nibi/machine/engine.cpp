#include "machine/engine.hpp"
#include "machine/byte_tools.hpp"
#include <fmt/format.h>

#include <iostream> // TODO: remove

namespace machine {

#define RAISE_ERROR(s) \
  print_instruction_data(ctx.instructions->data(), ctx.instructions->size()); \
  ctx.error_handler->on_error( \
    ctx.instruction_number, \
    execution_error_s{s}); \
  _engine_okay = false; \
  return;

void engine_c::reset_instruction_handling() {

  if (_print_result) {
    while(!_ctx.result_q.empty()) {
      fmt::print("{} ", _ctx.result_q.front().dump_to_string(true));
      _ctx.result_q.pop();
    }
    fmt::print("\n");
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

#define BINARY_OP(operation) \
{ \
  auto lhs = ctx.proc_q.front(); \
  ctx.proc_q.pop(); \
  auto rhs = ctx.proc_q.front(); \
  ctx.proc_q.pop(); \
  if (!(lhs.conditional_self_load(_scope.current()))) {\
      RAISE_ERROR(fmt::format("Unknown variable `{}`\n", lhs.dump_to_string(true))) \
  } \
  if (!(rhs.conditional_self_load(_scope.current()))) { \
      RAISE_ERROR(fmt::format("Unknown variable `{}`\n", rhs.dump_to_string(true))) \
  } \
  ctx.proc_q.push(lhs operation rhs); \
  break; \
}

void engine_c::execute(execution_ctx_s &ctx,instruction_view_s* iv) {
  switch((ins_id_e)iv->op) {
    case ins_id_e::NOP: break;
    case ins_id_e::EXEC_ADD: BINARY_OP(+);
    case ins_id_e::EXEC_SUB: BINARY_OP(-);
    case ins_id_e::EXEC_DIV: BINARY_OP(/);
    case ins_id_e::EXEC_MUL: BINARY_OP(*);
    case ins_id_e::EXEC_MOD: BINARY_OP(%);
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
      if (!ctx.proc_q.size()) {
        RAISE_ERROR(
          fmt::format(
            "Expected object type '{}' got 'none'\n",
             data_type_to_string(expected))); 
      }
      if (ctx.proc_q.front().type != expected) {
        RAISE_ERROR(
          fmt::format(
            "Expected object type '{}' got '{}'\n",
             data_type_to_string(expected),
             data_type_to_string(ctx.proc_q.front().type))); 
      }
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
      if (ctx.proc_q.size() != expected_size) {
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
        //fmt::print("moved: {} from proc_q to result_q\n", ctx.proc_q.front().dump_to_string(true));
        ctx.proc_q.pop();
      }
      break;
    }
    case ins_id_e::PUSH_RESULT: {
      ctx.proc_q.push(ctx.result_q.front());
      //fmt::print("moved: {} from result_q to proc_q\n", ctx.result_q.front().dump_to_string(true));
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
}


} // namespace
