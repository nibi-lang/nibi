#include "vm.hpp"
#include "nibi.hpp"
#include <algorithm>
#include <span>
#include "keywords.hpp"

namespace nibi {

  vm_c::vm_c(locator_table_c &locator_table)
    : ins_mem_observer_(*this),
      ins_data_(locator_table, ins_mem_observer_) {
  }

  bool vm_c::is_running() {
    return running_.load();
  }

  bool vm_c::start() {
#ifdef DEBUG_BUILD
    std::cout << "vm asked to start" << std::endl;
#endif
    if (is_running()) return true;

    // Expand the instruction by one block to start
    if (!ins_data_.expand_instruction_memory(1)) {
      return false;
    }

    running_.store(true);

    // Start threads or whatever here

    return true;
  }

  bool vm_c::stop() {

#ifdef DEBUG_BUILD
    std::cout << "vm asked to stop" << std::endl;
#endif
    if (!is_running()) return true;

    ins_data_.ins_block.clear();
    ins_data_.instructions = nullptr;

    return false;
  }

  void vm_c::instruction_memory_observer_c::alloc_failure_ind(
      const char* msg,
      size_t total_allocated,
      size_t attempted_alloc) {

    std::string error = "VM instruction memory OOM\nError: ";
    error += msg;
    error += "\n";
    error += std::to_string(total_allocated);
    error += " : bytes allocated before OOM\n";
    error += std::to_string(attempted_alloc);
    error += " : bytes attempted to allocate\n";

    fatal_error(error_origin_e::VM, nullptr, error);
  }

  locator_ptr vm_c::retrieve_locator(const size_t idx) {
    // Walk back from current instruction and find the closest locator
    size_t x = idx;
    while(x && ins_data_.instructions[x].op != bytecode::op_e::LOCATOR) {--x;}
    if (ins_data_.instructions[x].op == bytecode::op_e::LOCATOR) {
      return ins_data_.table.get_locator(
            ins_data_.instructions[x].data.i);
    }
    return nullptr;
  }

  void vm_c::bytecode_ind(std::vector<bytecode::instruction_s> & instructions) {
    ins_data_.insert_instructions(instructions);

    while (running_.load() && instruction_pointer_ < ins_data_.instruction_count) {
      instruction_pointer_ += execute_instruction(instruction_pointer_);
    }
  }

#define CURRENT_INS\
    ins_data_.instructions[idx]

#define PREVIOUS_INS\
    ins_data_.instructions[idx-1]

#define INS_OFFSET\
  ins_data_.instructions + idx

  #define LIST_LENGTH\
    (ins_data_.instructions + idx)->data.i

  size_t vm_c::execute_instruction(const size_t idx) {

      //std::cout << "Instruction pointer: " << instruction_pointer_ << std::endl;

      switch(CURRENT_INS.op) {
        case bytecode::op_e::NOP: { break; }
        case bytecode::op_e::LOCATOR: { break; }
        case bytecode::op_e::USER_SYMBOL: {

            break;
        }
        case bytecode::op_e::TAG: { break; }
        case bytecode::op_e::INTEGER: {
           std::cout << "Got integer literal : " << CURRENT_INS.data.i << std::endl;
           stack_.top().list.push_back(
               new trivial_object_c(CURRENT_INS.data.i));
           break;
        }
        case bytecode::op_e::REAL: { break; }
        case bytecode::op_e::CHAR: { break; }
        case bytecode::op_e::STRING: { break; }
        case bytecode::op_e::NIL: { break; }
        case bytecode::op_e::LIST_INS_IND: { 
          size_t accum = idx + 1;
          for(auto ins : std::span<bytecode::instruction_s> (INS_OFFSET+ 1, INS_OFFSET + LIST_LENGTH)) {
              accum += execute_instruction(accum);
          }
          return LIST_LENGTH;
        }
        case bytecode::op_e::LIST_END:
          execute_ctx(idx);
          break;
        case bytecode::op_e::LIST_DATA_IND: { break; }
        case bytecode::op_e::LIST_ACCESS_IND: { break; }
        case bytecode::op_e::BUILTIN_SYMBOL: {
           stack_.push(execution_ctx_s(CURRENT_INS.data.i));
           break;
        }

        /*
            Below are instructions that can not be generated
            directly from user data (parser)
        */

      } // end switch

      return 1;
  }

  void vm_c::execute_ctx(const size_t idx) {

          // Execute the top context
          // pop top context
          // push result of context
    switch (stack_.top().handler_id) {
      case kw::id::ADD: {
        std::cout << "DO THE ADD" << std::endl;
        break;
      }

      default:
        std::cerr << "UNHANDLED METHOD : " << stack_.top().handler_id << std::endl;
        fatal_error(error_origin_e::VM, retrieve_locator(idx), "Unhandled handler id");
      break;

    }
  }

} // namespace nibi
