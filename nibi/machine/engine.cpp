#include "machine/engine.hpp"
#include "machine/byte_tools.hpp"
#include <fmt/format.h>

#include <iostream> // TODO: remove

namespace machine {

void engine_c::reset_instruction_handling() {
  // Clear process frames and reset 

  std::cout << "Engine asked to reset instruction handling" << std::endl;
}

void engine_c::handle_instructions(
  const bytes_t& instructions,
  instruction_error_handler_if& error_handler) {

  std::cout << "Engine got "
            << instructions.size()
            << " bytes of instructions to execute\n";

  _ctx.pc = 0;
  _ctx.instructions = &instructions;
  _ctx.error_handler = &error_handler;

  execute_ctx(_ctx);
}

execution_error_s engine_c::generate_error(
  const std::string& msg) const {
  // TODO: Add other debug info`
  return {msg};
}

void engine_c::execute_ctx(execution_ctx_s &ctx) {

  const bytes_t& ins = *ctx.instructions;

  while(_engine_okay && 
        ctx.pc < ctx.instructions->size()) {

    auto& current_byte = ins[ctx.pc];

    if (current_byte >= (uint8_t)ins_id_e::ENUM_BOUNDARY) {
      // Push error object and return
      return;
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
  ctx.proc_q.push(lhs operation rhs); \
  break; \
}

#define RAISE_ERROR(s) \
  ctx.error_handler->on_error( \
    ctx.instruction_number, \
    execution_error_s{s}); \
  _engine_okay = false; \
  return;

void engine_c::execute(execution_ctx_s &ctx,instruction_view_s* iv) {
  switch((ins_id_e)iv->op) {
    case ins_id_e::NOP: break;

    case ins_id_e::PUSH_PROC_FRAME: break;
    case ins_id_e::POP_PROC_FRAME: break;
    case ins_id_e::EXEC_ADD: BINARY_OP(+);
    case ins_id_e::EXEC_SUB: BINARY_OP(-);
    case ins_id_e::EXEC_DIV: BINARY_OP(/);
    case ins_id_e::EXEC_MUL: BINARY_OP(*);
    case ins_id_e::EXEC_MOD: BINARY_OP(%);
    case ins_id_e::EXEC_ASSIGN: {
      auto target = ctx.proc_q.front();
      ctx.proc_q.pop();

      bool okay{false};
      if (!target.as_raw_str(okay) || !okay) {

        fmt::print("\tType: {}\t'{}'\n",
            data_type_to_string(target.type),
            target.to_string());


        RAISE_ERROR(
            "Expected identifier as first argument to assignment");
      }

      _scope.current()->insert(
          target.to_string(),
          ctx.proc_q.front());

      ctx.proc_q.pop();


      break;
    }
    case ins_id_e::EXPECT_OBJECT_TYPE: {
      data_type_e expected = (data_type_e)((uint8_t)(*(uint8_t*)(iv->data)));
      if (!ctx.proc_q.size()) {
        RAISE_ERROR(
          fmt::format(
            "Expected object type '{}' got 'none',",
             data_type_to_string(expected))); 
      }
      break;
    }
    case ins_id_e::EXPECT_N_ARGS: {
      uint8_t expected_size = *(uint8_t*)(iv->data);
      if (ctx.proc_q.size() != expected_size) {
        RAISE_ERROR(
          fmt::format(
           "Expected exactly '{}' arguments. Got '{}'.", 
           expected_size, ctx.proc_q.size()));

      }
      break;
    }
    case ins_id_e::EXPECT_GTE_N_ARGS: {
      uint8_t expected_size = *(uint8_t*)(iv->data);
      if (ctx.proc_q.size() != expected_size) {
        RAISE_ERROR(
          fmt::format(
            "Expected at least '{}' arguments. Got '{}'.",
            expected_size, ctx.proc_q.size()));
      }
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

      fmt::print("push {} len string: {}\n",
          iv->data_len,
          *(iv->data));
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
