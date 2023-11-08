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

  while(ctx.pc < ctx.instructions->size()) {

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

void engine_c::execute(execution_ctx_s &ctx,instruction_view_s* iv) {
  switch((ins_id_e)iv->op) {
    case ins_id_e::NOP: break;
    case ins_id_e::EXEC_ASSIGN: {
   //   std::string name((char*)iv->data, iv->data_len);
   //   _scope.current()->insert(name, ctx.proc_q.front());
   //   ctx.proc_q.pop();
   //
   //
   //     -- Add LOAD_SYMBOL, etc AND THEN we can do this
   //
   //
      break;
    }
    case ins_id_e::EXEC_ADD: BINARY_OP(+);
    case ins_id_e::EXEC_SUB: BINARY_OP(-);
    case ins_id_e::EXEC_DIV: BINARY_OP(/);
    case ins_id_e::EXEC_MUL: BINARY_OP(*);
    case ins_id_e::EXEC_MOD: BINARY_OP(%);
    case ins_id_e::EXPECT_N_ARGS: {
      uint64_t expected_size = *(uint8_t*)(iv->data);
      if (ctx.proc_q.size() != expected_size) {
        ctx.error_handler->on_error(
          ctx.instruction_number,
          execution_error_s{
              fmt::format(
                  "Expected exactly '{}' arguments. Got '{}'.", 
                  expected_size, ctx.proc_q.size())});
      }
      break;
    }
    case ins_id_e::EXPECT_GTE_N_ARGS: {
      uint64_t expected_size = *(uint8_t*)(iv->data);
      if (ctx.proc_q.size() != expected_size) {
        ctx.error_handler->on_error(
          ctx.instruction_number,
          execution_error_s{
              fmt::format(
                  "Expected at least '{}' arguments. Got '{}'.",
                  expected_size, ctx.proc_q.size())});
      }
      break;
    }
    case ins_id_e::LOAD_INT:
      ctx.proc_q.push(
        object_c::integer(*(int64_t*)(iv->data)));
      break;
    default: {
      std::cout << "OP:" << (int)iv->op << " not implemented yet\n";
    }
  }
}


} // namespace
